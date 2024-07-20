#pragma once
#include "ScriptDefines.h"
#include "ExecutionContextBase.h"

namespace FOC
{
	class Scene;

	struct ScriptExecutionContext : SCRIPT::ExecutionContextBase
	{
		Scene* const scene;
	};
}