#pragma once
#include "../ScriptDefines.h"
#include "ScriptCommand.h"
#include "ScriptFunctionCommand.h"
#include "ScriptCompositeCommand.h"
#include "../Utilities/ScriptUtilities.h"
#include <stack>

namespace SCR
{

	class CommandTracker final
	{
		friend class ScriptModifier;
		friend class ScriptInternalModifier;
	public:

		CommandTracker(Script& aScript);
		~CommandTracker();

		template<Predicate UndoPredicate, Predicate RedoPredicate>
		void Update(UndoPredicate&& aUndoPredicate, RedoPredicate&& aRedoPredicate);

		void Clear();

		bool& IsTracking();
		bool& IsDebugPrinting();
		const bool& IsDebugPrinting() const;

		size_t GetUndoSize() const;
		size_t GetRedoSize() const;

	private:


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

		std::shared_ptr<CompositeCommand> myCompositeCommand;

		Script& myScript;

		bool myIsTracking;
		bool myIsDebugPrinting;

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