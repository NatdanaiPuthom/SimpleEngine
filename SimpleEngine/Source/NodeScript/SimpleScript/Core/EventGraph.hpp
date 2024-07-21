#pragma once
#include "ScriptDefines.hpp"
#include "ScriptNodeGraph.hpp"

namespace FLY_NAMESPACE
{

	class EventGraph final : public NodeGraph
	{
	public:

		EventGraph();
		~EventGraph();

		void BindNodeToEvent(NodeID aNodeID);
		void UnbindNodeFromEvent(NodeID aNodeID);

	public:

		std::unordered_map<EventID, std::vector<NodeID>> mEventNodes;

	};
}