#include "ScriptCompositeCommand.h"
#include "../Script.h"

namespace SCR
{

	CompositeCommand::CompositeCommand(const std::string& aName)
		: myName(aName)
	{
	}

	CompositeCommand::~CompositeCommand()
	{

	}

	CompositeCommand::CompositeCommand(const CompositeCommand& aOther)
		: myCommands(aOther.myCommands)
		, myName(aOther.myName)
	{
		if (aOther.myCurrentChild)
		{
			myCurrentChild = std::make_unique<CompositeCommand>(*aOther.myCurrentChild);
		}
	}

	CompositeCommand::CompositeCommand(CompositeCommand&& aOther) noexcept
		: myCurrentChild(std::move(aOther.myCurrentChild))
		, myCommands(std::move(aOther.myCommands))
		, myName(std::move(aOther.myName))
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
			myCurrentChild = std::make_unique<CompositeCommand>(aName);
		}

	}

	CompositeCommand::eEndCode CompositeCommand::End()
	{
		if (myCurrentChild)
		{
			eEndCode endCode = myCurrentChild->End();
			if (endCode == eEndCode::Ended)
			{
				//CommandNew c = *myCurrentChild;
				const std::string compositeName = myCurrentChild->GetName();
				myCommands.emplace_back(Command(std::move(*myCurrentChild), compositeName));
				myCurrentChild.reset();
			}
			else if (endCode == eEndCode::Ended_Empty) // If the childs commands is empty we don't want to add the child to our commands
			{
				myCurrentChild.reset();
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
