#pragma once
#include "../FlyDefines.hpp"

namespace FLY_NAMESPACE
{
	template<typename T>
	struct NodeState final
	{
		T& mValue;
	};

	template<typename T>
	struct NodeExecutionContext final
	{
		const T& mContext;
	};

	struct ExecutionContextBase
	{
		float mDeltaTime = 0.0f;
	};
}