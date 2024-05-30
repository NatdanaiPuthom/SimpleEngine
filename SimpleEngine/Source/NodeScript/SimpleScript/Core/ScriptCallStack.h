#pragma once
#include "ScriptDefines.h"
#include <stack>

namespace SCR
{

	class NodeGraph;

	struct NodeCall
	{
		NodeID nodeID;
		NodeGraph* nodeGraph;
	};

	class CallStack final
	{
	public:


		CallStack();
		~CallStack();

		const Node& GetCurrentNode();
		
		void Push(NodeID aNodeID, NodeGraph* aNodeGraph);
		void Pop();

	private:

		std::stack<NodeCall> myCallStack;

	};
}

