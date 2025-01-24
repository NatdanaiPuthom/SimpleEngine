#include "Editor/Precomplied/EditorPch.hpp"
#include "EditorCompositeCommand.hpp"

namespace Editor
{

	CompositeCommand::CompositeCommand(std::string aName, std::vector<Command> aCommands)
		: myName(std::move(aName))
		, myCommands(std::move(aCommands))
	{
	}

	void CompositeCommand::Do() const
	{
		for (const Command& command : myCommands)
		{
			command.DoCommand();
		}
	}

	void CompositeCommand::Undo() const
	{
		for (int i = static_cast<int>(myCommands.size()) - 1; i >= 0; --i)
		{
			myCommands.at(i).UndoCommand();
		}
	}
}
