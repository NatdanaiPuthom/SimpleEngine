#pragma once
#include "../FlyDefines.hpp"
#include "../Utilities/FlyMeta.hpp"
#include <string>

namespace FLY_NAMESPACE
{

	template<typename T>
	concept Commandable = requires(const T & aData)
	{
		{ Do(aData) };
		{ Undo(aData) };
	};

	template<typename T>
	concept MemberCommandable = requires(const T & aData)
	{
		{ aData.Do() };
		{ aData.Undo() };
	};

	template<typename T>
	using FunctionType = void(*)(const T&);

	class CommandNew final
	{
	public:
		
		CommandNew() = default;

		template<typename T> requires Commandable<T> or MemberCommandable<T>
		CommandNew(const T& aData, const std::string& aName)
			: mConcept(std::make_unique<CommandModel<T>>(aData))
			, mName(aName)
		{
		}


		template<typename T, ValidCallable<void, const T&> DoFunc, ValidCallable<void, const T&> UndoFunc>
		CommandNew(const T& aData, DoFunc aDoFunction, UndoFunc aUndoFunction, const std::string& aName)
			: mConcept(std::make_unique<CommandModel<T, FunctionType<T>, FunctionType<T>>>(aData, aDoFunction, aUndoFunction))
			, mName(aName)
		{
		}


		CommandNew(const CommandNew& aOther)
			: mConcept(aOther.mConcept->Clone())
			, mName(aOther.mName)
		{

		}

		CommandNew(CommandNew&&) = default;

		CommandNew& operator=(const CommandNew& aOther)
		{
			CommandNew temp(aOther);
			std::swap(mConcept, temp.mConcept);
			mName = aOther.mName;
			return *this;
		}

		CommandNew& operator=(CommandNew&&) = default;

		void DoCommand() const;
		void UndoCommand() const;


	private:

		class CommandConcept
		{
		public:

			virtual ~CommandConcept() = default;

			virtual void DoCommand() const = 0;
			virtual void UndoCommand() const = 0;

			virtual std::unique_ptr<CommandConcept> Clone() const = 0;

		};

		template<typename... T>
		class CommandModel;

		template<Commandable T>
		class CommandModel<T> final : public CommandConcept
		{
		public:

			CommandModel(const T& aData)
				: mData(aData)
			{

			}

			void DoCommand() const override
			{
				Do(mData);
			}

			void UndoCommand() const override
			{
				Undo(mData);
			}

			std::unique_ptr<CommandConcept> Clone() const override
			{
				return std::make_unique<CommandModel<T>>(*this);
			}

		private:

			T mData;
		};

		template<MemberCommandable T>
		class CommandModel<T> final : public CommandConcept
		{
		public:

			CommandModel(const T& aData)
				: mData(aData)
			{

			}

			void DoCommand() const override
			{
				mData.Do();
			}

			void UndoCommand() const override
			{
				mData.Undo();
			}

			std::unique_ptr<CommandConcept> Clone() const override
			{
				return std::make_unique<CommandModel<T>>(*this);
			}

		private:

			T mData;
		};

		template<typename T>
		class CommandModel<T, FunctionType<T>, FunctionType<T>> final : public CommandConcept
		{
			using FunctionType = void(*)(const T&);
		public:

			CommandModel(const T& aData, FunctionType aDoFunction, FunctionType aUndoFunction)
				: mData(aData)
				, mDoFunction(aDoFunction)
				, mUndoFunction(aUndoFunction)
			{
			}

			void DoCommand() const override
			{
				mDoFunction(mData);
			}

			void UndoCommand() const override
			{
				mUndoFunction(mData);
			}

			std::unique_ptr<CommandConcept> Clone() const override
			{
				return std::make_unique<CommandModel>(*this);
			}

		private:

			T mData;
			FunctionType mDoFunction;
			FunctionType mUndoFunction;

		
		};


	private:


		std::unique_ptr<CommandConcept> mConcept;
		std::string mName;
	};

}