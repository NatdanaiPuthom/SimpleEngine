#include "Editor/Precomplied/EditorPch.hpp"
#include "EditorCompositeCommand.hpp"

namespace Editor
{

	CompositeCommand::CompositeCommand(std::string aName, std::vector<Command> aCommands)
		: myName(std::move(aName))
		, myCommands(std::move(aCommands))
	{
	}

	void CompositeCommand::Do(const bool aDebugPrint) const
	{
		for (const Command& command : myCommands)
		{
			command.DoCommand(aDebugPrint);
		}
	}

	void CompositeCommand::Undo(const bool aDebugPrint) const
	{
		for (int i = static_cast<int>(myCommands.size()) - 1; i >= 0; --i)
		{
			myCommands[i].UndoCommand(aDebugPrint);
		}
	}
}
