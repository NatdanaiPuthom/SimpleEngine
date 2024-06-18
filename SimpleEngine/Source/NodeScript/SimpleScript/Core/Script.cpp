#include "Script.h"
#include "Utilities/ScriptProxy.h"
#include "Node/NodeExecutor.h"
#include "Command/ScriptCommandTracker.h"
#include "ScriptUtilities.h"
#include "Instance/ScriptInstance.h"

namespace SCR
{

	Script::Script(ScriptManager& aScriptManager, const std::string& aName)
		: myName(aName)
		, myScriptManager(aScriptManager)
	{
	}

	Script::~Script()
	{
	}

	std::string& Script::Name()
	{
		return myName;
	}

	const std::string& Script::Name() const
	{
		return myName;
	}

	ScriptInstance* Script::CreateScriptInstance()
	{
		std::unique_ptr<ScriptInstance>& scriptInstance = myScriptInstances.emplace_back(std::make_unique<ScriptInstance>());
		scriptInstance->Compile(*this);
		
		return scriptInstance.get();
	}

	EventGraph& Script::GetEventGraph()
	{
		return myEventGraph;
	}
}

