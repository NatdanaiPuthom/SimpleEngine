#pragma once
#include "../FlyDefines.hpp"
#include "../Utilities/FlyMeta.hpp"
#include <string>
#include <functional>

namespace FLY_NAMESPACE
{

	enum class eCommandType
	{
		Do,
		Undo
	};

	class Command final
	{
		using CommandFunction = std::function<void(eCommandType)>;
	public:


		template<typename CallableCommand>
		Command(CallableCommand&& aCallableCommand, const std::string& aName)
			: mName(aName)
			, mCommandFunction(aCallableCommand)
		{
		}

		void operator()(eCommandType aCommandType) const;

	private:

		std::string mName;
		CommandFunction mCommandFunction;

	};

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
		
		template<typename T> requires Commandable<T> or MemberCommandable<T>
		CommandNew(T&& aData)
			: mConcept(std::make_unique<CommandModel<T>>(std::forward<T>(aData)))
		{
		}


		template<typename T, ValidCallable<void, const T&> DoFunc, ValidCallable<void, const T&> UndoFunc>
		CommandNew(T&& aData, DoFunc aDoFunction, UndoFunc aUndoFunction)
			: mConcept(std::make_unique<CommandModel<T, FunctionType<T>, FunctionType<T>>>(std::forward<T>(aData), aDoFunction, aUndoFunction))
		{
		}


		CommandNew(const CommandNew& aOther)
			: mConcept(aOther.mConcept->Clone())
		{

		}

		CommandNew(CommandNew&&) = default;

		CommandNew& operator=(const CommandNew& aOther)
		{
			CommandNew temp(aOther);
			std::swap(mConcept, temp.mConcept);
			return *this;
		}

		CommandNew& operator=(CommandNew&&) = default;

		void DoCommand()
		{
			mConcept->DoCommand();
		}

		void UndoCommand()
		{
			mConcept->UndoCommand();
		}


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

			CommandModel(T&& aData)
				: mData(std::forward<T>(aData))
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

			CommandModel(T&& aData)
				: mData(std::forward<T>(aData))
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
		class CommandModel<T, FunctionType<T>, FunctionType<T>>  final : public CommandConcept
		{
			using FunctionType = void(*)(const T&);
		public:

			CommandModel(T&& aData, FunctionType aDoFunction, FunctionType aUndoFunction)
				: mData(std::forward<T>(aData))
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
	};

}