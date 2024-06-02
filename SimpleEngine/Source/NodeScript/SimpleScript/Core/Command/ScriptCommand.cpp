#include "ScriptCommand.h"
#include "../Script.h"
#include "Utilities/ScriptProxy.h"
#include "ScriptCommandTracker.h"

namespace SCR
{
	Command::Command(const CommandContext& aContext)
		: Command(aContext, typeid(*this).name())
	{
	}

	Command::Command(const CommandContext& aContext, const std::string& aName)
		: myContext(aContext)
		, myName(aName)
	{
	}

	void Command::DoInternal()
	{
		if (ScriptProxy::GetCommandTracker(myContext.script).IsDebugPrinting() && ScriptProxy::GetCommandTracker(myContext.script).IsTracking())
		{
			std::cout << "Do Command: " << myName << std::endl;
		}
		Do();
	}

	void Command::UndoInternal()
	{
		if (ScriptProxy::GetCommandTracker(myContext.script).IsDebugPrinting() && ScriptProxy::GetCommandTracker(myContext.script).IsTracking())
		{
			std::cout << "Undo Command: " << myName << std::endl;
		}
		Undo();
	}


}
