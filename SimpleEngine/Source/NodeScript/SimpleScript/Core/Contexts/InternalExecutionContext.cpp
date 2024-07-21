#include "InternalExecutionContext.hpp"
#include "ScriptProxy.hpp"
#include "NodeExecutor.hpp"

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
			ScriptProxy::GetNodeExecutor().ExecuteNode(mExecutionQueue.front());
			mExecutionQueue.pop();
		}
	}
}
