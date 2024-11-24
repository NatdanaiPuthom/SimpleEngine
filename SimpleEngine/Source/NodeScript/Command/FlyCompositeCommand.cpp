#include "FlyCompositeCommand.hpp"

namespace FLY_NAMESPACE
{

	CompositeCommand::CompositeCommand(const std::string& aName)
		: mName(aName)
	{
	}

	void CompositeCommand::AddCommand(CommandNew&& aCommand)
	{
		if (mCurrentChild)
		{
			mCurrentChild->AddCommand(std::forward<CommandNew>(aCommand));
		}
		else
		{
			mCommands.emplace_back(std::forward<CommandNew>(aCommand));
		}
	}

	void CompositeCommand::Do() const
	{
		for (const CommandNew& command : mCommands)
		{
			command.DoCommand();
		}
	}

	void CompositeCommand::Undo() const
	{
		for (int i = static_cast<int>(mCommands.size()) - 1; i >= 0; --i)
		{
			mCommands.at(i).UndoCommand();
		}
	}

	void CompositeCommand::Begin(const std::string& aName)
	{
		if (mCurrentChild)
		{
			mCurrentChild->Begin(aName);
		}
		else
		{
			mCurrentChild = HeapObject<CompositeCommand, false>(aName);
		}

	}

	CompositeCommand::eEndCode CompositeCommand::End()
	{
		if (mCurrentChild)
		{
			const eEndCode endCode = mCurrentChild->End();
			if (endCode == eEndCode::Ended)
			{
				const std::string compositeName = mCurrentChild->GetName();
				mCommands.emplace_back(CommandNew(std::move(*mCurrentChild), compositeName));
				mCurrentChild.Reset();
			}
			else if (endCode == eEndCode::Ended_Empty) // If the child's commands are empty we don't want to add the child to our commands
			{
				mCurrentChild.Reset();
			}
			return eEndCode::InProgress;
		}
		else if (mCommands.empty())
		{
			return eEndCode::Ended_Empty;
		}

		return eEndCode::Ended;
	}

	const std::string& CompositeCommand::GetName()
	{
		return mName;
	}
}
