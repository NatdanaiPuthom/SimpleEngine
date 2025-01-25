#pragma once
#include <string>
#include <memory>

namespace Editor
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
	concept InternalMemberCommandable = requires(const T & aData)
	{
		{ aData.Do(true) };
		{ aData.Undo(true) };
	};

	template<typename T, typename Ret, typename... Args>
	concept ValidCallable = requires(T && aFunc, Args&&... args)
	{
		{ aFunc(std::forward<Args>(args)...) } -> std::same_as<Ret>;
	};

	template<typename T>
	using FunctionType = void(*)(const T&);

	class Command final
	{
	public:
		
		Command() = default;

		template<typename T> requires Commandable<T> || MemberCommandable<T> || InternalMemberCommandable<T>
		Command(const T& aData, const std::string& aName)
			: myConcept(std::make_unique<CommandModel<T>>(aData))
			, myName(aName)
		{
		}


		template<typename T, ValidCallable<void, const T&> DoFunc, ValidCallable<void, const T&> UndoFunc>
		Command(const T& aData, DoFunc aDoFunction, UndoFunc aUndoFunction, const std::string& aName)
			: myConcept(std::make_unique<CommandModel<T, FunctionType<T>, FunctionType<T>>>(aData, aDoFunction, aUndoFunction))
			, myName(aName)
		{
		}

		Command(const Command& aOther)
			: myConcept(aOther.myConcept ? aOther.myConcept->Clone() : nullptr)
			, myName(aOther.myName)
		{
		}

		Command(Command&&) = default;

		Command& operator=(const Command& aOther)
		{
			Command temp(aOther);
			std::swap(myConcept, temp.myConcept);
			myName = aOther.myName;
			return *this;
		}

		Command& operator=(Command&&) = default;

		void DoCommand(bool aDebugPrint) const;
		void UndoCommand(bool aDebugPrint) const;

	private:

		class CommandConcept
		{
		public:

			virtual ~CommandConcept() = default;

			virtual void DoCommand(bool aDebugPrint) const = 0;
			virtual void UndoCommand(bool aDebugPrint) const = 0;

			virtual std::unique_ptr<CommandConcept> Clone() const = 0;

		};

		template<typename... T>
		class CommandModel;

		template<Commandable T>
		class CommandModel<T> final : public CommandConcept
		{
		public:

			CommandModel(const T& aData)
				: myData(aData)
			{
			}

			void DoCommand(const bool) const override
			{
				Do(myData);
			}

			void UndoCommand(const bool) const override
			{
				Undo(myData);
			}

			std::unique_ptr<CommandConcept> Clone() const override
			{
				return std::make_unique<CommandModel<T>>(*this);
			}

		private:

			T myData;
		};

		template<MemberCommandable T>
		class CommandModel<T> final : public CommandConcept
		{
		public:

			CommandModel(const T& aData)
				: myData(aData)
			{
			}

			void DoCommand(const bool) const override
			{
				myData.Do();
			}

			void UndoCommand(const bool) const override
			{
				myData.Undo();
			}

			std::unique_ptr<CommandConcept> Clone() const override
			{
				return std::make_unique<CommandModel<T>>(*this);
			}

		private:

			T myData;
		};

		template<InternalMemberCommandable T>
		class CommandModel<T> final : public CommandConcept
		{
		public:

			CommandModel(const T& aData)
				: myData(aData)
			{
			}

			void DoCommand(const bool aDebugPrint) const override
			{
				myData.Do(aDebugPrint);
			}

			void UndoCommand(const bool aDebugPrint) const override
			{
				myData.Undo(aDebugPrint);
			}

			std::unique_ptr<CommandConcept> Clone() const override
			{
				return std::make_unique<CommandModel<T>>(*this);
			}

		private:

			T myData;
		};

		template<typename T>
		class CommandModel<T, FunctionType<T>, FunctionType<T>> final : public CommandConcept
		{
			using FunctionType = void(*)(const T&);
		public:

			CommandModel(const T& aData, FunctionType aDoFunction, FunctionType aUndoFunction)
				: myData(aData)
				, myDoFunction(aDoFunction)
				, myUndoFunction(aUndoFunction)
			{
			}

			void DoCommand(const bool aDebugPrint) const override
			{
				myDoFunction(myData);
			}

			void UndoCommand(const bool aDebugPrint) const override
			{
				myUndoFunction(myData);
			}

			std::unique_ptr<CommandConcept> Clone() const override
			{
				return std::make_unique<CommandModel>(*this);
			}

		private:

			T myData;
			FunctionType myDoFunction;
			FunctionType myUndoFunction;
		};

	private:

		std::unique_ptr<CommandConcept> myConcept;
		std::string myName;
	};

}