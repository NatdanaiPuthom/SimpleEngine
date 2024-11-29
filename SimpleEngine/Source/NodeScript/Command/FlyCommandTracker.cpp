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
			DoCommand(Command(*std::move(tempCommand), tempCommand->GetName()));
		}
		else if (endCode == CompositeCommand::eEndCode::Ended_Empty)
		{
			mCurrentCompositeCommand.Reset();
		}
	}

	void CommandTracker::UndoCommand()
	{
		if (!mUndoStack.empty())
		{
			HeapObject<Command>& topCommand = mUndoStack.top();
			topCommand->UndoCommand();
			mRedoStack.push(std::move(topCommand));
			mUndoStack.pop();
		}
	}

	void CommandTracker::RedoCommand()
	{
		if (!mRedoStack.empty())
		{
			HeapObject<Command>& topCommand = mRedoStack.top();
			topCommand->DoCommand();
			mUndoStack.push(std::move(topCommand));
			mRedoStack.pop();
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
}

