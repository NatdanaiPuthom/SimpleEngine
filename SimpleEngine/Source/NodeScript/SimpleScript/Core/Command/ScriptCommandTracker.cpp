#include "ScriptCommandTracker.h"

namespace SCR
{
	CommandTracker::CommandTracker(Script& aScript)
		: myScript(aScript)
		, myIsTracking(true)
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
		if (myCompositeCommand)
		{
			myCompositeCommand->Begin(aName);
		}
		else
		{
			myCompositeCommand = std::make_shared<CompositeCommand>(aName);
		}
	}

	void CommandTracker::EndComposite()
	{
		CompositeCommand::eEndCode endCode = myCompositeCommand->End();
		
		if (endCode == CompositeCommand::eEndCode::Ended)
		{
			std::shared_ptr<CompositeCommand> composite = myCompositeCommand;
			myCompositeCommand.reset();
			DoCommand(composite, true);
		}
		else if (endCode == CompositeCommand::eEndCode::Ended_Empty)
		{
			myCompositeCommand.reset();
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
		if (myIsTracking && myCompositeCommand)
		{
			myCompositeCommand->AddCommand(aCommand);
			return;
		}
		if (aExecuteCommand)
		{
			aCommand->DoInternal(myScript);
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
			myUndoStack.top()->UndoInternal(myScript);
			myRedoStack.push(myUndoStack.top());
			myUndoStack.pop();
		}
	}

	void CommandTracker::RedoCommand()
	{
		if (!myRedoStack.empty())
		{
			myRedoStack.top()->DoInternal(myScript);
			myUndoStack.push(myRedoStack.top());
			myRedoStack.pop();
		}
	}
}

