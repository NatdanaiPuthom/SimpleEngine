#include "FlyCompositeCommand.hpp"

namespace FLY_NAMESPACE
{

	CompositeCommand::CompositeCommand(const std::string& aName)
		: myName(aName)
	{
	}

	void CompositeCommand::AddCommand(Command&& aCommand)
	{
		if (myCurrentChild)
		{
			myCurrentChild->AddCommand(std::forward<Command>(aCommand));
		}
		else
		{
			myCommands.emplace_back(std::forward<Command>(aCommand));
		}
	}

	void CompositeCommand::operator()(eCommandType aCommandType) const
	{
		switch (aCommandType)
		{
		case eCommandType::Do:
			Do();
			break;
		case eCommandType::Undo:
			Undo();
			break;
		}
	}

	void CompositeCommand::Begin(const std::string& aName)
	{
		if (myCurrentChild)
		{
			myCurrentChild->Begin(aName);
		}
		else
		{
			myCurrentChild = HeapObject<CompositeCommand, true>(aName);
		}

	}

	CompositeCommand::eEndCode CompositeCommand::End()
	{
		if (myCurrentChild)
		{
			const eEndCode endCode = myCurrentChild->End();
			if (endCode == eEndCode::Ended)
			{
				const std::string compositeName = myCurrentChild->GetName();
				myCommands.emplace_back(Command(std::move(*myCurrentChild), compositeName));
				myCurrentChild.Reset();
			}
			else if (endCode == eEndCode::Ended_Empty) // If the child's commands are empty we don't want to add the child to our commands
			{
				myCurrentChild.Reset();
			}
			return eEndCode::InProgress;
		}
		else if (myCommands.empty())
		{
			return eEndCode::Ended_Empty;
		}

		return eEndCode::Ended;
	}

	const std::string& CompositeCommand::GetName()
	{
		return myName;
	}

	void CompositeCommand::Do() const
	{
		for (const Command& command : myCommands)
		{
			command(eCommandType::Do);
		}
	}

	void CompositeCommand::Undo() const
	{
		for (int i = static_cast<int>(myCommands.size()) - 1; i >= 0; --i)
		{
			myCommands.at(i)(eCommandType::Undo);
		}
	}
}
