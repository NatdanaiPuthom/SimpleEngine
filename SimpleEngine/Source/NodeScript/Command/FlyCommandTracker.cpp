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
		mUndoStack = {};
		mRedoStack = {};
	}

	void CommandTracker::DoCommand(CommandNew&& aCommand)
	{
		DoCommandInternal(std::forward<CommandNew>(aCommand), true);
	}

	void CommandTracker::RegisterCommand(CommandNew&& aCommand)
	{
		DoCommandInternal(std::forward<CommandNew>(aCommand), false);
	}

	void CommandTracker::BeginComposite(const std::string& aName)
	{
		if (mCurrentCompositeCommand)
		{
			mCurrentCompositeCommand->Begin(aName);
		}
		else
		{
			mCurrentCompositeCommand = HeapObject<CompositeCommand, false>(aName);
		}
	}

	void CommandTracker::EndComposite()
	{
		const CompositeCommand::eEndCode endCode = mCurrentCompositeCommand->End();

		if (endCode == CompositeCommand::eEndCode::Ended)
		{
			HeapObject<CompositeCommand, false> tempCommand = std::move(mCurrentCompositeCommand);
			mCurrentCompositeCommand.Reset();
			DoCommand(CommandNew(*std::move(tempCommand), tempCommand->GetName()));
		}
		else if (endCode == CompositeCommand::eEndCode::Ended_Empty)
		{
			mCurrentCompositeCommand.Reset();
		}
	}

	size_t CommandTracker::GetUndoSize() const
	{
		return mUndoStack.size();
	}

	size_t CommandTracker::GetRedoSize() const
	{
		return mRedoStack.size();
	}

	void CommandTracker::UndoCommand()
	{
		if (!mUndoStack.empty())
		{
			HeapObject<CommandNew>& topCommand = mUndoStack.top();
			topCommand->UndoCommand();
			mRedoStack.push(std::move(topCommand));
			mUndoStack.pop();
		}
	}

	void CommandTracker::RedoCommand()
	{
		if (!mRedoStack.empty())
		{
			HeapObject<CommandNew>& topCommand = mRedoStack.top();
			topCommand->DoCommand();
			mUndoStack.push(std::move(topCommand));
			mRedoStack.pop();
		}
	}

	void CommandTracker::DoCommandInternal(CommandNew&& aCommand, bool aExecute)
	{
		if (mCurrentCompositeCommand)
		{
			mCurrentCompositeCommand->AddCommand(std::forward<CommandNew>(aCommand));
			return;
		}

		if (aExecute)
		{
			aCommand.DoCommand();
		}

		mUndoStack.push(HeapObject<CommandNew>(std::forward<CommandNew>(aCommand)));

		mRedoStack = {};
	}
}

