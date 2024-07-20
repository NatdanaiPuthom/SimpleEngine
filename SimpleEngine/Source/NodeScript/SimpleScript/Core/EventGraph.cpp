#include "EventGraph.hpp"
#include "Utilities/ScriptUtilities.hpp"
#include "Node/NodeTypeManager.hpp"
#include "Global/ScriptGlobal.hpp"

namespace SCR
{
	EventGraph::EventGraph()
		: NodeGraph(eNodeGraphType::Event)
	{
	}

	EventGraph::~EventGraph()
	{
	}

	void EventGraph::BindNodeToEvent(const NodeID aNodeID)
	{
		const Node& node = mNodeManager->mNodes.at(aNodeID);
		const NodeType& nodeType = Global::GetNodeTypeManager().GetNodeType(node.mTypeID);

		const EventID mEventID = nodeType.mNodeRecipe.mEventID;

		if (mEventID == InvalidID<EventID>())
		{
			return;
		}

		bool alreadyExists = false;
		std::vector<NodeID>& nodeIDsByEvent = myEventNodes[mEventID];
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
		const Node& node = mNodeManager->mNodes.at(aNodeID);
		const NodeType& nodeType = Global::GetNodeTypeManager().GetNodeType(node.mTypeID);

		auto it = myEventNodes.find(nodeType.mNodeRecipe.mEventID);

		if (it != myEventNodes.end())
		{
			std::erase(it->second, aNodeID);
		}
	}
}
