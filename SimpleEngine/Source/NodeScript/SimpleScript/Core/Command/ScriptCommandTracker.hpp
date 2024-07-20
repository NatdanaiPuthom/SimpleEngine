#pragma once
#include "../ScriptDefines.hpp"
#include "ScriptCommand.hpp"
#include <stack>

namespace FLY_NAMESPACE
{

	class CompositeCommand;

	class CommandTracker final
	{
	public:

		CommandTracker();
		~CommandTracker();

		void Clear();

		size_t GetUndoSize() const;
		size_t GetRedoSize() const;

	public:

		void DoCommand(Command&& aCommand);
		void RegisterCommand(Command&& aCommand);

		void BeginComposite(const std::string& aName);
		void EndComposite();

		void UndoCommand();
		void RedoCommand();

	private:

		void DoCommandInternal(Command&& aCommand, bool aExecute);

	private:


		std::stack<std::unique_ptr<Command>> myUndoStack;
		std::stack<std::unique_ptr<Command>> myRedoStack;
		std::unique_ptr<CompositeCommand> myCurrentCompositeCommand;

	};
}