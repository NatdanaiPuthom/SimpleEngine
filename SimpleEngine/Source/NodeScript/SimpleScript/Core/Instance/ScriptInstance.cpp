#include "ScriptInstance.h"
#include "../Script.h"
#include "ScriptProxy.h"
#include "NodeExecutor.h"
#include <ScriptUtilities.h>

namespace SCR
{

	ScriptInstance::ScriptInstance()
	{
	}

	ScriptInstance::~ScriptInstance()
	{
	}

	void ScriptInstance::Init(Script& aScript)
	{
		myScript = &aScript;
		myVariableManagerInstance.Compile(ScriptProxy::GetVariableManager(aScript));
	}

	void ScriptInstance::ExecuteEvent(eNodeEventType anExecutionTrait, const ExecutionContextBase& aContext)
	{
		ScriptProxy::GetNodeExecutor().ExecuteEvent(EnumCast(anExecutionTrait), myScript->GetEventGraph(), *this, aContext);
	}
}
