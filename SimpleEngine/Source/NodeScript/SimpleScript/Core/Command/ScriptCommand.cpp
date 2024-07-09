#include "ScriptCommand.h"
#include "ScriptCommandTracker.h"
#include "../Global/ScriptGlobal.h"

namespace SCR
{
	Command::Command(const Command& aOther)
		: myName(aOther.myName)
		, myCommandFunction(aOther.myCommandFunction)
	{
	}

	Command::Command(Command&& aOther)
		: myName(std::move(aOther.myName))
		, myCommandFunction(std::move(aOther.myCommandFunction))
	{
	}

	void Command::operator()(eCommandType aCommandType) const
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
