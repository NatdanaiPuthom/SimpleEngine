#include "FlyNodeExecutionQueue.hpp"
#include "Global/FlyGlobal.hpp"
#include "FlyNodeExecutor.hpp"

namespace FLY_NAMESPACE
{

	ExecutionQueue::ExecutionQueue()
	{
	}

	ExecutionQueue::~ExecutionQueue()
	{
	}

	void ExecutionQueue::Push(const NodeExecutionData& aNode)
	{
		mExecutionQueue.push(aNode);
	}

	void ExecutionQueue::Execute()
	{
		while (!mExecutionQueue.empty())
		{
			Global::GetNodeExecutor().ExecuteNode(mExecutionQueue.front());
			mExecutionQueue.pop();
		}
	}
}
