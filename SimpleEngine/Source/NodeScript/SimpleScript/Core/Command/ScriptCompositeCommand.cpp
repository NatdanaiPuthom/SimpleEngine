#include "ScriptCompositeCommand.h"
#include "../Script.h"

namespace SCR
{
	CompositeCommand::CompositeCommand(const std::string& aName)
		: Command({}, "Composite: " + aName)
	{
	}

	CompositeCommand::~CompositeCommand()
	{
	}

	void CompositeCommand::AddCommand(std::shared_ptr<Command> aCommand)
	{
		if (myCurrentChild)
		{
			myCurrentChild->AddCommand(aCommand);
		}
		else
		{
			myCommands.push_back(aCommand);
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
				myCommands.push_back(std::move(myCurrentChild));
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
		for (const std::shared_ptr<Command>& command : myCommands)
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

}
