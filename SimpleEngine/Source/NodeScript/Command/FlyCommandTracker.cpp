#include "FlyCommandTracker.hpp"
#include "FlyCompositeCommand.hpp"

namespace FLY_NAMESPACE
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
			myCurrentCompositeCommand = HeapObject<CompositeCommand, true>(aName);
		}
	}

	void CommandTracker::EndComposite()
	{
		const CompositeCommand::eEndCode endCode = myCurrentCompositeCommand->End();

		if (endCode == CompositeCommand::eEndCode::Ended)
		{
			HeapObject<CompositeCommand, true> tempCommand = std::move(myCurrentCompositeCommand);
			myCurrentCompositeCommand.Reset();
			DoCommand(Command(*std::move(tempCommand), tempCommand->GetName()));
		}
		else if (endCode == CompositeCommand::eEndCode::Ended_Empty)
		{
			myCurrentCompositeCommand.Reset();
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
			HeapObject<Command, false>& topCommand = myUndoStack.top();
			(*topCommand)(eCommandType::Undo);
			myRedoStack.push(std::move(topCommand));
			myUndoStack.pop();
		}
	}

	void CommandTracker::RedoCommand()
	{
		if (!myRedoStack.empty())
		{
			HeapObject<Command, false>& topCommand = myRedoStack.top();
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

		myUndoStack.push(HeapObject<Command, false>(std::forward<Command>(aCommand)));

		while (!myRedoStack.empty())
		{
			myRedoStack.pop();
		}
	}
}

