#include "ScriptInstance.h"
#include "../Script.h"
#include "ScriptProxy.h"
#include "NodeExecutor.h"
#include <ScriptUtilities.h>

namespace SCR
{

	ScriptInstance::ScriptInstance()
		: myScript(nullptr)
	{
	}

	ScriptInstance::~ScriptInstance()
	{
	}

	void ScriptInstance::Init(Script& aScript)
	{
		myScript = &aScript;
		myVariableManagerInstance.Init(ScriptProxy::GetVariableManager(aScript));
		// TODO Fix
		myEventGraphInstance.Init(aScript.GetEventGraph());
	}
}
