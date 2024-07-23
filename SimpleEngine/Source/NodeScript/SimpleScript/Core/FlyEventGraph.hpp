#pragma once
#include "ScriptDefines.hpp"
#include "FlyNodeGraph.hpp"

namespace FLY_NAMESPACE
{

	class EventGraph final
	{
	public:

		void BindNodeToEvent(NodeID aNodeID);
		void UnbindNodeFromEvent(NodeID aNodeID);

	public:

		NodeGraph mNodeGraph;
		std::unordered_map<EventID, std::vector<NodeID>> mEventNodes;
	};
}