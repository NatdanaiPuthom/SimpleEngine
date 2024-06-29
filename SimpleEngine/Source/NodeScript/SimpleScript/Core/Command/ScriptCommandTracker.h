#pragma once
#include "../ScriptDefines.h"
#include "ScriptCommand.h"
#include "../Utilities/MetaScript.h"
#include <stack>

namespace SCR
{

	class CompositeCommand;
	class CompositeCommandNew;

	class CommandTracker final
	{
	public:

		CommandTracker();
		~CommandTracker();

		template<Predicate UndoPredicate, Predicate RedoPredicate>
		void Update(UndoPredicate&& aUndoPredicate, RedoPredicate&& aRedoPredicate);

		void Clear();

		size_t GetUndoSize() const;
		size_t GetRedoSize() const;

	public:


		/*template<IsBaseOf<Command> CommandType, typename... Args> requires HasArgsConstructor<CommandType, Args...>
		void DoCommand(Args&&... args);

		template<IsBaseOf<Command> CommandType, typename... Args> requires HasArgsConstructor<CommandType, Args...>
		void RegisterCommand(Args&&... args);*/


		void DoCommand(CommandNew&& aCommand);
		void RegisterCommand(CommandNew&& aCommand);

		void BeginComposite(const std::string& aName);
		void EndComposite();

		//void DoCommand(std::unique_ptr<Command> aCommand, bool aExecuteCommand);

		void UndoCommand();
		void RedoCommand();

	private:

		void DoCommandInternal(CommandNew&& aCommand, bool aExecute);

	private:

		/*std::stack<std::unique_ptr<Command>> myUndoStack;
		std::stack<std::unique_ptr<Command>> myRedoStack;


		std::unique_ptr<CompositeCommand> myCurrentCompositeCommand;*/


		std::stack<std::unique_ptr<CommandNew>> myUndoStackNew;
		std::stack<std::unique_ptr<CommandNew>> myRedoStackNew;
		std::unique_ptr<CompositeCommandNew> myCurrentCompositeCommandNew;

	};

	template<Predicate UndoPredicate, Predicate RedoPredicate>
	inline void CommandTracker::Update(UndoPredicate&& aUndoPredicate, RedoPredicate&& aRedoPredicate)
	{
		if (aUndoPredicate())
		{
			UndoCommand();
		}
		else if (aRedoPredicate())
		{
			RedoCommand();
		}
	}

	//template<IsBaseOf<Command> CommandType, typename... Args> requires HasArgsConstructor<CommandType, Args...>
	//inline void CommandTracker::DoCommand(Args&&... args)
	//{
	//	std::unique_ptr<Command> command = std::make_unique<CommandType>(std::forward<Args>(args)...);
	//	DoCommand(std::move(command), true);
	//}

	//template<IsBaseOf<Command> CommandType, typename... Args> requires HasArgsConstructor<CommandType, Args...>
	//inline void CommandTracker::RegisterCommand(Args&&... args)
	//{
	//	std::unique_ptr<Command> command = std::make_unique<CommandType>(std::forward<Args>(args)...);
	//	DoCommand(std::move(command), false);
	//}
}