//#include "NodeScript/Precomplied/NodeScriptPch.hpp"
#include "InternalExecutionContext.h"
#include "ScriptProxy.h"
#include "NodeExecutor.h"

namespace SCR
{
	const Node& InternalExecutionContext::GetCurrentNode() const
	{
		return ScriptProxy::GetNode(*nodeData.nodeRef.nodeGraph, nodeData.nodeRef.nodeID);
	}

	ExecutionQueue::ExecutionQueue()
	{
	}

	ExecutionQueue::~ExecutionQueue()
	{
	}

	void ExecutionQueue::Push(const NodeExecutionData& aNode)
	{
		myExecutionQueue.push(aNode);
	}

	void ExecutionQueue::Execute(NodeExecutor& aNodeExecutor)
	{
		while (!myExecutionQueue.empty())
		{
			aNodeExecutor.ExecuteNode(myExecutionQueue.front());
			myExecutionQueue.pop();
		}
	}
}
