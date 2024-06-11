#include "ScriptCommandTracker.h"
#include "ScriptCompositeCommand.h"

namespace SCR
{
	CommandTracker::CommandTracker()
		: myIsTracking(true)
		, myIsDebugPrinting(false)
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

	void CommandTracker::BeginComposite(const std::string& aName)
	{
		if (myCurrentCompositeCommand)
		{
			myCurrentCompositeCommand->Begin(aName);
		}
		else
		{
			myCurrentCompositeCommand = std::make_shared<CompositeCommand>(aName);
		}
	}

	void CommandTracker::EndComposite()
	{
		CompositeCommand::eEndCode endCode = myCurrentCompositeCommand->End();
		
		if (endCode == CompositeCommand::eEndCode::Ended)
		{
			std::shared_ptr<CompositeCommand> composite = myCurrentCompositeCommand;
			myCurrentCompositeCommand.reset();
			DoCommand(composite, true);
		}
		else if (endCode == CompositeCommand::eEndCode::Ended_Empty)
		{
			myCurrentCompositeCommand.reset();
		}
	}

	bool& CommandTracker::IsTracking()
	{
		return myIsTracking;
	}

	bool& CommandTracker::IsDebugPrinting()
	{
		return myIsDebugPrinting;
	}

	const bool& CommandTracker::IsDebugPrinting() const
	{
		return myIsDebugPrinting;
	}


	size_t CommandTracker::GetUndoSize() const
	{
		return myUndoStack.size();
	}

	size_t CommandTracker::GetRedoSize() const
	{
		return myRedoStack.size();
	}

	void CommandTracker::DoCommand(std::shared_ptr<Command> aCommand, bool aExecuteCommand)
	{
		if (myIsTracking && myCurrentCompositeCommand)
		{
			myCurrentCompositeCommand->AddCommand(aCommand);
			return;
		}
		if (aExecuteCommand)
		{
			aCommand->DoInternal();
		}

		if (myIsTracking)
		{
			myUndoStack.push(aCommand);

			while (!myRedoStack.empty())
			{
				myRedoStack.pop();
			}
		}
	}

	void CommandTracker::UndoCommand()
	{
		if (!myUndoStack.empty())
		{
			myUndoStack.top()->UndoInternal();
			myRedoStack.push(myUndoStack.top());
			myUndoStack.pop();
		}
	}

	void CommandTracker::RedoCommand()
	{
		if (!myRedoStack.empty())
		{
			myRedoStack.top()->DoInternal();
			myUndoStack.push(myRedoStack.top());
			myRedoStack.pop();
		}
	}
}

