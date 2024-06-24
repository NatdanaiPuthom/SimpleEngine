#pragma once
#include "../ScriptDefines.h"
#include "NodeManagerInstance.h"

namespace SCR
{

	class NodeGraphInstance final
	{
	public:

		NodeGraphInstance();
		~NodeGraphInstance();

		void Init(const NodeGraph& aNodeGraph);

		NodeManagerInstance myNodeManagerInstance;

	};
}