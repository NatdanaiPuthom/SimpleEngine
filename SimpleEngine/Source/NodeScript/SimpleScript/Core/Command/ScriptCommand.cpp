#include "ScriptCommand.h"
#include "../Script.h"

namespace SCR
{
	Command::Command()
		: myName(typeid(*this).name())
	{
	}

	Command::Command(const std::string& aName)
		: myName(aName)
	{
	}

	void Command::DoInternal(Script& aScript)
	{
		if (ScriptProxy::GetCommandTracker(aScript).IsDebugPrinting() && ScriptProxy::GetCommandTracker(aScript).IsTracking())
		{
			std::cout << "Do Command: " << myName << std::endl;
		}
		Do(aScript);
	}

	void Command::UndoInternal(Script& aScript)
	{
		if (ScriptProxy::GetCommandTracker(aScript).IsDebugPrinting() && ScriptProxy::GetCommandTracker(aScript).IsTracking())
		{
			std::cout << "Undo Command: " << myName << std::endl;
		}
		Undo(aScript);
	}


}
