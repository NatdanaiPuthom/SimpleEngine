#pragma once
#include "../FlyDefines.hpp"
#include "../FlyRegistration.hpp"

namespace FLY_NAMESPACE
{
	std::tuple<Flow, float> Tick(NodeExecutionContext<ExecutionContextBase> aContext);
	Flow BeginPlay();
	Flow EndPlay();
}