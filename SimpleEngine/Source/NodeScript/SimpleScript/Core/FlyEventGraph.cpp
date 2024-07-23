#include "FlyEventGraph.hpp"
#include "Utilities/ScriptUtilities.hpp"
#include "Node/NodeTypeManager.hpp"
#include "Global/FlyGlobal.hpp"

namespace FLY_NAMESPACE
{

	void EventGraph::BindNodeToEvent(const NodeID aNodeID)
	{
		const Node& node = mNodeGraph.mNodes.at(aNodeID);
		const NodeType& nodeType = Global::GetNodeTypeManager().GetNodeType(node.mTypeID);

		const EventID eventID = nodeType.mNodeRecipe.mEventID;

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
		const Node& node = mNodeGraph.mNodes.at(aNodeID);
		const NodeType& nodeType = Global::GetNodeTypeManager().GetNodeType(node.mTypeID);

		auto it = mEventNodes.find(nodeType.mNodeRecipe.mEventID);

		if (it != mEventNodes.end())
		{
			std::erase(it->second, aNodeID);
		}
	}
}
