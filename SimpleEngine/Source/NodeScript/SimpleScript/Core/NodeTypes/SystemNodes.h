#pragma once
#include "ScriptDefines.h"
#include "ExecutionNodes.h"
#include "OperatorNodes.h"
#include "UtilityNodes.h"
#include "MathNodes.h"
#include "RandomNodes.h"

namespace SCR
{

	class NodeTypeRegistry;
	
	inline void RegisterSystemNodes()
	{
		RegisterExecutionNodes();
		RegisterUtilityNodes();
		RegisterMathNodes();
		RegisterRandomNodes();
	}
}