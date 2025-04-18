#include "FlyEventGraph.hpp"
#include "Utilities/FlyUtilities.hpp"
#include "Node/FlyNodeTypeManager.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{

	void EventGraph::BindNodeToEvent(const NodeID aNodeID)
	{
		const Node& node = mNodeGraph.GetNode(aNodeID);
		const NodeType& nodeType = Internal::GetNodeTypeManager().GetNodeType(node.GetTypeID());

		const EventID eventID = nodeType.GetEventID();

		if (eventID == InvalidID<EventID>())
		{
			return;
		}

		bool alreadyExists = false;
		std::vector<NodeID>& nodeIDsByEvent = mEventNodes[eventID];
		for (const NodeID mNodeID : nodeIDsByEvent)
		{
			if (mNodeID == aNodeID)
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


	void EventGraph::UnbindNodeFromEvent(const NodeID aNodeID)
	{
		const Node& node = mNodeGraph.GetNode(aNodeID);
		const NodeType& nodeType = Internal::GetNodeTypeManager().GetNodeType(node.GetTypeID());

		auto it = mEventNodes.find(nodeType.GetEventID());

		if (it != mEventNodes.end())
		{
			std::erase(it->second, aNodeID);
		}
	}
}
