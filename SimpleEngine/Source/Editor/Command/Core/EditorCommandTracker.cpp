#include "Editor/Precomplied/EditorPch.hpp"
#include "EditorCommandTracker.hpp"
#include "EditorCompositeCommand.hpp"

namespace Editor
{
	CommandTracker::CommandTracker()
	{
	}

	void CommandTracker::Clear()
	{
		myUndoStack = {};
		myRedoStack = {};
	}

	void CommandTracker::DoCommand(Command aCommand)
	{
		DoCommandInternal(true, std::move(aCommand));
	}

	void CommandTracker::RegisterCommand(Command aCommand)
	{
		DoCommandInternal(false, std::move(aCommand));
	}

	void CommandTracker::BeginComposite(std::string_view aName)
	{
		myCompositeCommandBuilder.Begin(aName);
	}

	void CommandTracker::EndComposite()
	{
		std::optional<CompositeCommand> compositeCommand = myCompositeCommandBuilder.End();

		if (compositeCommand)
		{
			DoCommand(Command(compositeCommand.value(), compositeCommand->GetName()));
		}
	}

	void CommandTracker::UndoCommand(const bool aDebugPrint)
	{
		if (myUndoStack.empty())
		{
			return;
		}
		Command& topCommand = myUndoStack.top();
		topCommand.UndoCommand(aDebugPrint);
		myRedoStack.push(std::move(topCommand));
		myUndoStack.pop();
	}

	void CommandTracker::RedoCommand(const bool aDebugPrint)
	{
		if (myRedoStack.empty())
		{
			return;
		}
		Command& topCommand = myRedoStack.top();
		topCommand.DoCommand(aDebugPrint);
		myUndoStack.push(std::move(topCommand));
		myRedoStack.pop();
	}

	size_t CommandTracker::GetUndoSize() const
	{
		return myUndoStack.size();
	}

	size_t CommandTracker::GetRedoSize() const
	{
		return myRedoStack.size();
	}

	void CommandTracker::DoCommandInternal(const bool aExecute, Command&& aCommand)
	{
		if (myCompositeCommandBuilder.IsActive())
		{
			myCompositeCommandBuilder.AddCommand(std::move(aCommand));
			return;
		}

		if (aExecute)
		{
			aCommand.DoCommand(false);
		}

		myUndoStack.push(std::move(aCommand));

		myRedoStack = {};
	}
}

