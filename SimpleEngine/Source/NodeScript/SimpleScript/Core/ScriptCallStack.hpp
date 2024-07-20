#pragma once
#include "ScriptDefines.hpp"
#include "Node/NodeRef.hpp"
#include <stack>

namespace FLY_NAMESPACE
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

