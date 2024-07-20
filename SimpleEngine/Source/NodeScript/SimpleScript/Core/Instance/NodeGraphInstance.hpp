#pragma once
#include "../ScriptDefines.hpp"
#include "NodeManagerInstance.hpp"

namespace SCR
{

	class NodeGraphInstance final
	{
	public:

		NodeGraphInstance();
		~NodeGraphInstance();

		void Init(const NodeGraph& aNodeGraph);

		NodeManagerInstance mNodeManagerInstance;

	};
}