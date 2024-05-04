#pragma once
#include "../ScriptDefines.h"

namespace SCR
{
	template<typename T>
	struct NodeState final
	{
		T& value;
	};

	template<typename T>
	struct NodeExecutionContext
	{
		const T& context;
	};
}