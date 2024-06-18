#pragma once
#include "ScriptDefines.h"
#include "ScriptNodeGraph.h"

namespace SCR
{

	class EventGraph final
	{
	public:

		EventGraph();
		~EventGraph();

		void BindNodeToEvent(NodeID aNodeID);
		void UnbindNodeFromEvent(NodeID aNodeID);

	public:

		NodeGraph myNodeGraph;
		std::unordered_map<EventID, std::vector<NodeID>> myEventNodes;

	};
}