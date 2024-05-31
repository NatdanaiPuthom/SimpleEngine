#include "ScriptCallStack.h"
#include "ScriptNodeGraph.h"
#include "ScriptProxy.h"

namespace SCR
{
	CallStack::CallStack()
	{
	}

	CallStack::~CallStack()
	{
	}

	const Node& CallStack::GetCurrentNode()
	{
		assert(!myCallStack.empty());

		const NodeRef& nodeRef = myCallStack.top();
		return ScriptProxy::GetNode(*nodeRef.nodeGraph, nodeRef.nodeID);
	}

	void CallStack::Push(NodeID aNodeID, NodeGraph* aNodeGraph)
	{
		myCallStack.push({ aNodeID, aNodeGraph });
	}


	void CallStack::Pop()
	{
		myCallStack.pop();
	}

}

