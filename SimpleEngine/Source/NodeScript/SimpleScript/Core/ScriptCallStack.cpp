#include "ScriptCallStack.hpp"
#include "ScriptNodeGraph.hpp"
#include "ScriptProxy.hpp"

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
		return ScriptProxy::GetNode(*nodeRef.mNodeGraph, nodeRef.mNodeID);
	}

	void CallStack::Push(NodeID aNodeID, NodeGraph& aNodeGraph)
	{
		myCallStack.push(NodeRef{ aNodeID, &aNodeGraph });
	}


	void CallStack::Pop()
	{
		myCallStack.pop();
	}

}

