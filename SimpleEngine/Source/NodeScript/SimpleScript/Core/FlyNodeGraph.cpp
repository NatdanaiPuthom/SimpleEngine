#include "FlyNodeGraph.hpp"
#include "Global/FlyGlobal.hpp"
#include "Node/NodeTypeManager.hpp"

namespace FLY_NAMESPACE
{

	NodeGraph::~NodeGraph()
	{
		for (NodeID nodeID = 0; nodeID < mNodes.size(); ++nodeID)
		{
			const Node& node = mNodes[nodeID];
			NodeType& nodeType = Global::GetNodeTypeManager().GetNodeType(node.mTypeID);
			std::erase(nodeType.mNodeRefs, NodeRef{ .mNodeID = nodeID, .mNodeGraph = this });
		}
	}

	NodeGraph::NodeGraph(const NodeGraph& aOther)
		: mNodes(aOther.mNodes)
		, mPins(aOther.mPins)
		, mMemoryArena(aOther.mMemoryArena)
	{

		for (Pin& pin : mPins)
		{
			pin.mDataPtr = mMemoryArena.GetRenewedPointer(pin.mDataPtr, aOther.mMemoryArena);
		}
	}
}