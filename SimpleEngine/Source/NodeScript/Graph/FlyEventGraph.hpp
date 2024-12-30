#pragma once
#include "../FlyDefines.hpp"
#include "FlyNodeGraph.hpp"
#include <unordered_map>

namespace FLY_NAMESPACE
{
	
	constexpr EventID AutoTickEventID{ 0 };

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