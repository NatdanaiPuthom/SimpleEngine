#include "EventGraph.h"
#include "ScriptUtilities.h"
#include <NodeTypeManager.h>

namespace SCR
{
	EventGraph::EventGraph()
	{
	}
	EventGraph::~EventGraph()
	{
	}
	void EventGraph::BindNodeToEvent(const NodeID aNodeID)
	{
		const Node& node = myNodeManager->myNodes.at(aNodeID);
		const NodeType& nodeType = NodeTypeManager::GetInstance().GetNodeType(node.typeID);

		const EventID eventID = nodeType.nodeRecipe.eventID;

		if (eventID == InvalidID<EventID>())
		{
			return;
		}

		bool alreadyExists = false;
		std::vector<NodeID>& nodeIDsByEvent = myEventNodes[eventID];
		for (const NodeID nodeID : nodeIDsByEvent)
		{
			if (nodeID == aNodeID)
			{
				alreadyExists = true;
				break;
			}
		}

		if (!alreadyExists)
		{
			nodeIDsByEvent.push_back(aNodeID);
		}

	}


	void EventGraph::UnbindNodeFromEvent(NodeID aNodeID)
	{
		const Node& node = myNodeManager->myNodes.at(aNodeID);
		const NodeType& nodeType = NodeTypeManager::GetInstance().GetNodeType(node.typeID);

		auto it = myEventNodes.find(nodeType.nodeRecipe.eventID);

		if (it != myEventNodes.end())
		{
			std::erase(it->second, aNodeID);
		}
	}
}
