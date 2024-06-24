#pragma once
#include "../ScriptDefines.h"
#include "ScriptCommand.h"
#include "../Utilities/MetaScript.h"
#include <stack>

namespace SCR
{

	class CompositeCommand;

	class CommandTracker final
	{
		friend class ScriptModifier;
		friend class InternalModifier;
	public:

		CommandTracker();
		~CommandTracker();

		template<Predicate UndoPredicate, Predicate RedoPredicate>
		void Update(UndoPredicate&& aUndoPredicate, RedoPredicate&& aRedoPredicate);

		void Clear();

		size_t GetUndoSize() const;
		size_t GetRedoSize() const;

	public:


		template<IsBaseOf<Command> CommandType, typename... Args> requires HasArgsConstructor<CommandType, Args...>
		void DoCommand(Args&&... args);

		template<IsBaseOf<Command> CommandType, typename... Args> requires HasArgsConstructor<CommandType, Args...>
		void RegisterCommand(Args&&... args);

		void BeginComposite(const std::string& aName);
		void EndComposite();

		void DoCommand(std::shared_ptr<Command> aCommand, bool aExecuteCommand);

		void UndoCommand();
		void RedoCommand();

	private:

		std::stack<std::shared_ptr<Command>> myUndoStack;
		std::stack<std::shared_ptr<Command>> myRedoStack;

		std::shared_ptr<CompositeCommand> myCurrentCompositeCommand;

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

	template<IsBaseOf<Command> CommandType, typename... Args> requires HasArgsConstructor<CommandType, Args...>
	inline void CommandTracker::DoCommand(Args&&... args)
	{
		std::shared_ptr<Command> command = std::make_shared<CommandType>(std::forward<Args>(args)...);
		DoCommand(command, true);
	}

	template<IsBaseOf<Command> CommandType, typename... Args> requires HasArgsConstructor<CommandType, Args...>
	inline void CommandTracker::RegisterCommand(Args&&... args)
	{
		std::shared_ptr<Command> command = std::make_shared<CommandType>(std::forward<Args>(args)...);
		DoCommand(command, false);
	}
}