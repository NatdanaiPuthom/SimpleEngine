#include "ScriptCommandTracker.h"
#include "ScriptCompositeCommand.h"

namespace SCR
{
	CommandTracker::CommandTracker()
	{
	}

	CommandTracker::~CommandTracker()
	{
	}

	void CommandTracker::Clear()
	{
		while (!myUndoStack.empty())
		{
			myUndoStack.pop();
		}
		while (!myRedoStack.empty())
		{
			myRedoStack.pop();
		}
	}

	void CommandTracker::DoCommand(Command&& aCommand)
	{
		DoCommandInternal(std::forward<Command>(aCommand), true);
	}

	void CommandTracker::RegisterCommand(Command&& aCommand)
	{
		DoCommandInternal(std::forward<Command>(aCommand), false);
	}

	void CommandTracker::BeginComposite(const std::string& aName)
	{
		if (myCurrentCompositeCommand)
		{
			myCurrentCompositeCommand->Begin(aName);
		}
		else
		{
			myCurrentCompositeCommand = std::make_unique<CompositeCommand>(aName);
		}
	}

	void CommandTracker::EndComposite()
	{
		CompositeCommand::eEndCode endCode = myCurrentCompositeCommand->End();

		if (endCode == CompositeCommand::eEndCode::Ended)
		{
			std::unique_ptr<CompositeCommand> composite = std::move(myCurrentCompositeCommand);
			myCurrentCompositeCommand.reset();
			DoCommand(Command(std::move(*composite), composite->GetName()));
		}
		else if (endCode == CompositeCommand::eEndCode::Ended_Empty)
		{
			myCurrentCompositeCommand.reset();
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

	void CommandTracker::UndoCommand()
	{
		if (!myUndoStack.empty())
		{
			std::unique_ptr<Command>& topCommand = myUndoStack.top();
			(*topCommand)(eCommandType::Undo);
			myRedoStack.push(std::move(topCommand));
			myUndoStack.pop();
		}
	}

	void CommandTracker::RedoCommand()
	{
		if (!myRedoStack.empty())
		{
			std::unique_ptr<Command>& topCommand = myRedoStack.top();
			(*topCommand)(eCommandType::Do);
			myUndoStack.push(std::move(topCommand));
			myRedoStack.pop();
		}
	}

	void CommandTracker::DoCommandInternal(Command&& aCommand, bool aExecute)
	{
		if (myCurrentCompositeCommand)
		{
			myCurrentCompositeCommand->AddCommand(std::forward<Command>(aCommand));
			return;
		}

		if (aExecute)
		{
			aCommand(eCommandType::Do);
		}

		myUndoStack.push(std::make_unique<Command>(std::forward<Command>(aCommand)));

		while (!myRedoStack.empty())
		{
			myRedoStack.pop();
		}
	}
}

