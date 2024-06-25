#include "ScriptCompositeCommand.h"
#include "../Script.h"

namespace SCR
{
	CompositeCommand::CompositeCommand(const std::string& aName)
		: Command("Composite: " + aName)
	{
	}

	CompositeCommand::~CompositeCommand()
	{
	}

	void CompositeCommand::AddCommand(std::unique_ptr<Command> aCommand)
	{
		if (myCurrentChild)
		{
			myCurrentChild->AddCommand(std::move(aCommand));
		}
		else
		{
			myCommands.emplace_back(std::move(aCommand));
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
				myCommands.emplace_back(std::move(myCurrentChild));
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

	void CompositeCommand::Do()
	{
		for (const std::unique_ptr<Command>& command : myCommands)
		{
			command->DoInternal();
		}
	}

	void CompositeCommand::Undo()
	{
		for (int i = static_cast<int>(myCommands.size()) - 1; i >= 0; --i)
		{
			myCommands.at(i)->UndoInternal();
		}
	}

	//////


	CompositeCommandNew::CompositeCommandNew(const std::string& aName)
		: myName(aName)
	{
	}

	CompositeCommandNew::~CompositeCommandNew()
	{

	}

	CompositeCommandNew::CompositeCommandNew(const CompositeCommandNew& aOther)
		: myCommands(aOther.myCommands)
		, myName(aOther.myName)
	{
		if (aOther.myCurrentChild)
		{
			myCurrentChild = std::make_unique<CompositeCommandNew>(*aOther.myCurrentChild);
		}
	}

	CompositeCommandNew::CompositeCommandNew(CompositeCommandNew&& aOther) noexcept
		: myCurrentChild(std::move(aOther.myCurrentChild))
		, myCommands(std::move(aOther.myCommands))
		, myName(std::move(aOther.myName))
	{

	}

	void CompositeCommandNew::AddCommand(CommandNew&& aCommand)
	{
		if (myCurrentChild)
		{
			myCurrentChild->AddCommand(std::move(aCommand));
		}
		else
		{
			myCommands.emplace_back(std::move(aCommand));
		}
	}
	void CompositeCommandNew::operator()(eCommandType aCommandType) const
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
	void CompositeCommandNew::Begin(const std::string& aName)
	{
		if (myCurrentChild)
		{
			myCurrentChild->Begin(aName);
		}
		else
		{
			myCurrentChild = std::make_unique<CompositeCommandNew>(aName);
		}

	}

	CompositeCommandNew::eEndCode CompositeCommandNew::End()
	{
		if (myCurrentChild)
		{
			eEndCode endCode = myCurrentChild->End();
			if (endCode == eEndCode::Ended)
			{
				//CommandNew c = *myCurrentChild;
				const std::string compositeName = myCurrentChild->GetName();
				myCommands.emplace_back(CommandNew(std::move(*myCurrentChild), compositeName));
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
}
