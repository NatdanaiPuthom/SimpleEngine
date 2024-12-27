#include "FlyNodeExecutionQueue.hpp"
#include "FlyNodeExecutor.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{

	void ExecutionQueue::Push(const NodeExecutionData& aNode)
	{
		mExecutionQueue.push(aNode);
	}

	void ExecutionQueue::Execute()
	{
		while (!mExecutionQueue.empty())
		{
			Internal::GetNodeExecutor().ExecuteNode(mExecutionQueue.front());
			mExecutionQueue.pop();
		}
	}
}
