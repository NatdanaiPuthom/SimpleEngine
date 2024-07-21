#include "FlyNodeGraphView.hpp"
#include "ScriptNodeGraph.hpp"

namespace FLY_NAMESPACE
{

	NodeGraphView::NodeGraphView(NodeGraph& aNodeGraph)
		: mNodeGraph(&aNodeGraph)
	{
	}

	std::vector<NodeView> NodeGraphView::GetNodes(const bool aIncludeDestroyed) const
	{
		const std::vector<Node>& nodes = mNodeGraph->mNodes;

		std::vector<NodeView> nodeViews;
		nodeViews.reserve(nodes.size());

		for (NodeID nodeID = 0; nodeID < nodes.size(); ++nodeID)
		{
			NodeView nodeView(nodeID, *mNodeGraph);
			if (!aIncludeDestroyed && nodeView.IsDestroyed())
			{
				continue;
			}
			nodeViews.push_back(nodeView);
		}

		return nodeViews;
	}

	std::vector<PinView> NodeGraphView::GetPins(bool aIncludeDestroyed) const
	{
		const std::vector<Pin>& pins = mNodeGraph->mPins;

		std::vector<PinView> pinViews;
		pinViews.reserve(pins.size());

		for (PinID pinID = 0; pinID < pins.size(); ++pinID)
		{
			PinView pinView(pinID, *mNodeGraph);
			NodeView nodeView(pinView.GetNodeID(), *mNodeGraph);
			if (!aIncludeDestroyed && nodeView.IsDestroyed())
			{
				continue;
			}
			pinViews.push_back(pinView);
		}

		return pinViews;
	}

	NodeGraph& NodeGraphView::GetNodeGraph()
	{
		return *mNodeGraph;
	}
}
