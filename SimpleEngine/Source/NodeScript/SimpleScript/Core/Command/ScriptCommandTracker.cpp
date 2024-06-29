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
		/*while (!myUndoStack.empty())
		{
			myUndoStack.pop();
		}
		while (!myRedoStack.empty())
		{
			myRedoStack.pop();
		}*/

		while (!myUndoStackNew.empty())
		{
			myUndoStackNew.pop();
		}
		while (!myRedoStackNew.empty())
		{
			myRedoStackNew.pop();
		}
	}

	void CommandTracker::DoCommand(CommandNew&& aCommand)
	{
		DoCommandInternal(std::move(aCommand), true);
	}

	void CommandTracker::RegisterCommand(CommandNew&& aCommand)
	{
		DoCommandInternal(std::move(aCommand), false);
	}

	void CommandTracker::BeginComposite(const std::string& aName)
	{
		if (myCurrentCompositeCommandNew)
		{
			myCurrentCompositeCommandNew->Begin(aName);
		}
		else
		{
			myCurrentCompositeCommandNew = std::make_unique<CompositeCommandNew>(aName);
		}
		/*if (myCurrentCompositeCommand)
		{
			myCurrentCompositeCommand->Begin(aName);
		}
		else
		{
			myCurrentCompositeCommand = std::make_unique<CompositeCommand>(aName);
		}*/
	}

	void CommandTracker::EndComposite()
	{
		CompositeCommandNew::eEndCode endCode = myCurrentCompositeCommandNew->End();

		if (endCode == CompositeCommandNew::eEndCode::Ended)
		{
			std::unique_ptr<CompositeCommandNew> composite = std::move(myCurrentCompositeCommandNew);
			myCurrentCompositeCommandNew.reset();
			DoCommand(CommandNew(std::move(*composite), composite->GetName()));
		}
		else if (endCode == CompositeCommandNew::eEndCode::Ended_Empty)
		{
			myCurrentCompositeCommandNew.reset();
		}
		/*CompositeCommand::eEndCode endCode = myCurrentCompositeCommand->End();

		if (endCode == CompositeCommand::eEndCode::Ended)
		{
			std::unique_ptr<CompositeCommand> composite = std::move(myCurrentCompositeCommand);
			myCurrentCompositeCommand.reset();
			DoCommand(std::move(composite), true);
		}
		else if (endCode == CompositeCommand::eEndCode::Ended_Empty)
		{
			myCurrentCompositeCommand.reset();
		}*/
	}

	size_t CommandTracker::GetUndoSize() const
	{
		return myUndoStackNew.size();
		//return myUndoStack.size();
	}

	size_t CommandTracker::GetRedoSize() const
	{
		return myRedoStackNew.size();
		//return myRedoStack.size();
	}

	/*void CommandTracker::DoCommand(std::unique_ptr<Command> aCommand, bool aExecuteCommand)
	{
		if (myCurrentCompositeCommand)
		{
			myCurrentCompositeCommand->AddCommand(std::move(aCommand));
			return;
		}
		if (aExecuteCommand)
		{
			aCommand->DoInternal();
		}

		myUndoStack.push(std::move(aCommand));

		while (!myRedoStack.empty())
		{
			myRedoStack.pop();
		}

	}*/

	void CommandTracker::UndoCommand()
	{
		if (!myUndoStackNew.empty())
		{
			std::unique_ptr<CommandNew>& topCommand = myUndoStackNew.top();
			(*topCommand)(eCommandType::Undo);
			myRedoStackNew.push(std::move(topCommand));
			myUndoStackNew.pop();
		}
		/*if (!myUndoStack.empty())
		{
			std::unique_ptr<Command>& topCommand = myUndoStack.top();
			topCommand->UndoInternal();
			myRedoStack.push(std::move(topCommand));
			myUndoStack.pop();
		}*/
	}

	void CommandTracker::RedoCommand()
	{
		if (!myRedoStackNew.empty())
		{
			std::unique_ptr<CommandNew>& topCommand = myRedoStackNew.top();
			(*topCommand)(eCommandType::Do);
			myUndoStackNew.push(std::move(topCommand));
			myRedoStackNew.pop();
		}
		/*if (!myRedoStack.empty())
		{
			std::unique_ptr<Command>& topCommand = myRedoStack.top();
			topCommand->DoInternal();
			myUndoStack.push(std::move(topCommand));
			myRedoStack.pop();
		}*/
	}
	void CommandTracker::DoCommandInternal(CommandNew&& aCommand, bool aExecute)
	{
		if (myCurrentCompositeCommandNew)
		{
			myCurrentCompositeCommandNew->AddCommand(std::forward<CommandNew>(aCommand));
			return;
		}

		if (aExecute)
		{
			aCommand(eCommandType::Do);
		}

		myUndoStackNew.push(std::make_unique<CommandNew>(std::forward<CommandNew>(aCommand)));

		while (!myRedoStackNew.empty())
		{
			myRedoStackNew.pop();
		}
	}
}

