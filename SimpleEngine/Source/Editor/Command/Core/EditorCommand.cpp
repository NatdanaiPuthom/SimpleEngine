#include "Editor/Precomplied/EditorPch.hpp"
#include "EditorCommand.hpp"

namespace Editor
{

	void Command::ExecuteCommand(const bool aDebugPrint) const
	{
		if (aDebugPrint)
		{
			std::cout << "Do Command: " << myName << std::endl;
		}
		myConcept->ExecuteCommand(aDebugPrint);
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
