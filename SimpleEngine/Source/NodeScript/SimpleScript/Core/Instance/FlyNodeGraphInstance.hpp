#pragma once
#include "../FlyDefines.hpp"
#include "FlyNodeManagerInstance.hpp"

namespace FLY_NAMESPACE
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