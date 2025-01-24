#include "Editor/Precomplied/EditorPch.hpp"
#include "EditorCommand.hpp"

namespace Editor
{

	void Command::DoCommand() const
	{
		if (false)
		{
			std::cout << "Do Command: " << myName << std::endl;
		}
		myConcept->DoCommand();
	}

	void Command::UndoCommand() const
	{
		if (false)
		{
			std::cout << "Undo Command: " << myName << std::endl;
		}
		myConcept->UndoCommand();
	}
}
