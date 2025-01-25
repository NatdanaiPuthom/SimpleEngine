#include "Editor/Precomplied/EditorPch.hpp"
#include "EditorCommand.hpp"

namespace Editor
{

	void Command::DoCommand(const bool aDebugPrint) const
	{
		if (aDebugPrint)
		{
			std::cout << "Do Command: " << myName << std::endl;
		}
		myConcept->DoCommand(aDebugPrint);
	}

	void Command::UndoCommand(const bool aDebugPrint) const
	{
		if (aDebugPrint)
		{
			std::cout << "Undo Command: " << myName << std::endl;
		}
		myConcept->UndoCommand(aDebugPrint);
	}
}
