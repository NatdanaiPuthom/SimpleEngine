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
		
		void Push(const NodeRef& aNodeRef);
		NodeRef Pop();

	private:

		std::stack<NodeRef> mCallStack;

	};
}

