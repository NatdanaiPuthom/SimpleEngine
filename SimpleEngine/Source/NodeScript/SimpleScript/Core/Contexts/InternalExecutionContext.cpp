#include "InternalExecutionContext.hpp"
#include "ScriptProxy.hpp"
#include "NodeExecutor.hpp"

namespace SCR
{
	const Node& InternalExecutionContext::GetCurrentNode() const
	{
		return ScriptProxy::GetNode(*mNodeData.mNodeRef.mNodeGraph, mNodeData.mNodeRef.mNodeID);
	}

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
