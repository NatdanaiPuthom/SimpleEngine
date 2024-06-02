#pragma once
#include "ScriptDefines.h"
#include "Node/NodeRef.h"
#include <stack>

namespace SCR
{

	class NodeGraph;

	class CallStack final
	{
	public:


		CallStack();
		~CallStack();

		const Node& GetCurrentNode();
		
		void Push(NodeID aNodeID, NodeGraph& aNodeGraph);
		void Pop();

	private:

		std::stack<NodeRef> myCallStack;

	};
}

