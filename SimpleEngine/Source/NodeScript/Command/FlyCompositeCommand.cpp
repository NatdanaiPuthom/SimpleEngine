#include "FlyCompositeCommand.hpp"

namespace FLY_NAMESPACE
{

	CompositeCommand::CompositeCommand(std::string aName)
		: mName(std::move(aName))
	{
	}

	void CompositeCommand::AddCommand(Command&& aCommand)
	{
		if (mCurrentChild)
		{
			mCurrentChild->AddCommand(std::move(aCommand));
		}
		else
		{
			mCommands.emplace_back(std::move(aCommand));
		}
	}

	void CompositeCommand::Do() const
	{
		for (const Command& command : mCommands)
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

	void CompositeCommand::Begin(std::string_view aName)
	{
		if (mCurrentChild)
		{
			mCurrentChild->Begin(aName);
		}
		else
		{
			mCurrentChild = HeapObject<CompositeCommand, false>(std::string(aName));
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
				mCommands.emplace_back(Command(std::move(*mCurrentChild), compositeName));
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

	const std::string& CompositeCommand::GetName() const
	{
		return mName;
	}
}
