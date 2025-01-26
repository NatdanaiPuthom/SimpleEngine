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

		void ExecuteCommand(Command aCommand);

		// Register Command without executing it
		void RegisterCommand(Command aCommand);

		void BeginComposite(std::string_view aName);
		void EndComposite();

		void UndoCommand(const bool aDebugPrint = false);
		void RedoCommand(const bool aDebugPrint = false);

		[[nodiscard]] size_t GetUndoSize() const;
		[[nodiscard]] size_t GetRedoSize() const;

	private:

		void ExecuteCommandInternal(bool aExecute, Command&& aCommand);

	private:

		std::stack<Command> myUndoStack;
		std::stack<Command> myRedoStack;
		CompositeCommandBuilder myCompositeCommandBuilder;

	};
}