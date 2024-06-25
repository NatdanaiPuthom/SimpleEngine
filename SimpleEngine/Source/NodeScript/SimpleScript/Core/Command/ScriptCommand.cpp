#include "ScriptCommand.h"
#include "../Script.h"
#include "Utilities/ScriptProxy.h"
#include "ScriptCommandTracker.h"

namespace SCR
{
	Command::Command()
		: Command(typeid(*this).name())
	{
	}

	Command::Command(const std::string& aName)
		: myName(aName)
	{
	}

	void Command::DoInternal()
	{
		//std::cout << "Do Command: " << myName << std::endl;
		Do();
	}

	void Command::UndoInternal()
	{
		//std::cout << "Undo Command: " << myName << std::endl;
		Undo();
	}


	void CommandNew::operator()(eCommandType aCommandType) const
	{
		if (aCommandType == eCommandType::Do)
		{
			std::cout << "Do Command: " << myName << std::endl;
		}
		else
		{
			std::cout << "Undo Command: " << myName << std::endl;
		}
		myCommandFunction(aCommandType);
	}

}
