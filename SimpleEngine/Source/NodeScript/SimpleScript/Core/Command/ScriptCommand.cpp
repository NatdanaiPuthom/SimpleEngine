#include "ScriptCommand.h"
#include "ScriptCommandTracker.h"
#include "../Global/ScriptGlobal.h"

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
		Do();
	}

	void Command::UndoInternal()
	{
		Undo();
	}


	void CommandNew::operator()(eCommandType aCommandType) const
	{
		if (Global::IsDebugging())
		{
			if (aCommandType == eCommandType::Do)
			{
				std::cout << "Do Command: " << myName << std::endl;
			}
			else
			{
				std::cout << "Undo Command: " << myName << std::endl;
			}
		}
		myCommandFunction(aCommandType);
	}

}
