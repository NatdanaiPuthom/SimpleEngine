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
		if (mCurrentCompositeCommand)
		{
			mCurrentCompositeCommand->Begin(aName);
		}
		else
		{
			mCurrentCompositeCommand = HeapObject<CompositeCommand, false>(std::string(aName));
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
			Command& topCommand = mUndoStack.top();
			topCommand.UndoCommand();
			mRedoStack.push(std::move(topCommand));
			mUndoStack.pop();
		}
	}

	void CommandTracker::RedoCommand()
	{
		if (!mRedoStack.empty())
		{
			Command& topCommand = mRedoStack.top();
			topCommand.DoCommand();
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

	void CommandTracker::DoCommandInternal(const bool aExecute, Command&& aCommand)
	{
		if (mCurrentCompositeCommand)
		{
			mCurrentCompositeCommand->AddCommand(std::move(aCommand));
			return;
		}

		if (aExecute)
		{
			aCommand.DoCommand();
		}

		mUndoStack.push(std::move(aCommand));

		mRedoStack = {};
	}
}

