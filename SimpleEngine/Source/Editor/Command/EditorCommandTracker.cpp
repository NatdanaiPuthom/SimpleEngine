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

		/*if (endCode == CompositeCommand::eEndCode::Ended)
		{
			std::unique_ptr<CompositeCommand> tempCommand = std::move(myCurrentCompositeCommand);
			myCurrentCompositeCommand.reset();
			DoCommand(Command(*std::move(tempCommand), tempCommand->GetName()));
		}
		else if (endCode == CompositeCommand::eEndCode::Ended_Empty)
		{
			myCurrentCompositeCommand.reset();
		}*/
	}

	void CommandTracker::UndoCommand()
	{
		if (!myUndoStack.empty())
		{
			Command& topCommand = myUndoStack.top();
			topCommand.UndoCommand();
			myRedoStack.push(std::move(topCommand));
			myUndoStack.pop();
		}
	}

	void CommandTracker::RedoCommand()
	{
		if (!myRedoStack.empty())
		{
			Command& topCommand = myRedoStack.top();
			topCommand.DoCommand();
			myUndoStack.push(std::move(topCommand));
			myRedoStack.pop();
		}
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
			aCommand.DoCommand();
		}

		myUndoStack.push(std::move(aCommand));

		myRedoStack = {};
	}
}

