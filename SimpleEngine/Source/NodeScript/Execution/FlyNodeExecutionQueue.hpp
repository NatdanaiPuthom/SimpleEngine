#pragma once
#include "../FlyDefines.hpp"
#include "FlyInternalExecutionContext.hpp"
#include <queue>

namespace FLY_NAMESPACE
{

	class ExecutionQueue final
	{
	public:

		ExecutionQueue();
		~ExecutionQueue();

		void Push(const NodeExecutionData& aNode);
		void Execute();

	private:


		std::queue<NodeExecutionData> mExecutionQueue;

	};
}
