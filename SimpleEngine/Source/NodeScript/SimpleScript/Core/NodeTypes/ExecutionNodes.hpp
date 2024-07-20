#pragma once
#include "../ScriptDefines.hpp"
#include "../Contexts/ExecutionContextBase.hpp"
#include "../SystemTypes/ScriptFlow.hpp"
#include "../Node/NodeState.hpp"

namespace FLY_NAMESPACE
{
	std::tuple<Flow, float> Tick(NodeExecutionContext<ExecutionContextBase> aContext);
	Flow BeginPlay();
	Flow EndPlay();
}