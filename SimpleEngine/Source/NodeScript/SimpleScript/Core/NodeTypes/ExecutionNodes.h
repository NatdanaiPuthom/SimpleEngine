#pragma once
#include "../ScriptDefines.h"
#include "../Contexts/ExecutionContextBase.h"
#include "../SystemTypes/ScriptFlow.h"
#include "../Node/NodeState.h"

namespace SCR
{
	std::tuple<Flow, float> Tick(NodeExecutionContext<ExecutionContextBase> aContext);
	Flow BeginPlay();
	Flow EndPlay();
}