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
		const Node& node = myNodeGraph.myNodeManager->myNodes.at(aNodeID);
		const NodeType& nodeType = NodeTypeManager::GetNodeType(node.typeID);

		const EventID eventID = nodeType.nodeRecipe.eventID;

		if (eventID != EnumCast(eNodeEventType::None))
		{
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
	}


	void EventGraph::UnbindNodeFromEvent(NodeID aNodeID)
	{
		const Node& node = myNodeGraph.myNodeManager->myNodes.at(aNodeID);
		const NodeType& nodeType = NodeTypeManager::GetNodeType(node.typeID);

		std::erase(myEventNodes.at(nodeType.nodeRecipe.eventID), aNodeID);
	}
}
