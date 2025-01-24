#pragma once
#include "EditorCommand.hpp"
#include "EditorCompositeCommand.hpp"
#include "EditorCompositeCommandBuilder.hpp"
#include <stack>
#include <memory>

namespace Editor
{

	class CommandTracker final
	{
	public:

		CommandTracker();

		void Clear();


		void DoCommand(Command aCommand);

		void RegisterCommand(Command aCommand);

		void BeginComposite(std::string_view aName);
		void EndComposite();

		void UndoCommand();
		void RedoCommand();

		size_t GetUndoSize() const;
		size_t GetRedoSize() const;

	private:

		void DoCommandInternal(bool aExecute, Command&& aCommand);

	private:


		std::stack<Command> myUndoStack;
		std::stack<Command> myRedoStack;
		CompositeCommandBuilder myCompositeCommandBuilder;

	};
}