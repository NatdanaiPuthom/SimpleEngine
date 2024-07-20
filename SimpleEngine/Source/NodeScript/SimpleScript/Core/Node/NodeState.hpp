#pragma once
#include "../ScriptDefines.hpp"

namespace FLY_NAMESPACE
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