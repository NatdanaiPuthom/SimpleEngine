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

		const NodeCall& call = myCallStack.top();
		return ScriptProxy::GetNode(*call.nodeGraph, call.nodeID);
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

