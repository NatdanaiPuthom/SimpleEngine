#include "ScriptNodeGraph.hpp"
#include "Global/FlyGlobal.hpp"
#include "Node/NodeTypeManager.hpp"

namespace FLY_NAMESPACE
{

	NodeGraph::NodeGraph(const eNodeGraphType aType)
		: mType(aType)
	{
	}

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
		, mType(aOther.mType)
	{

		for (Pin& pin : mPins)
		{
			pin.mDataPtr = mMemoryArena.GetRenewedPointer(pin.mDataPtr, aOther.mMemoryArena);
		}
	}
	eNodeGraphType NodeGraph::GetType() const
	{
		return mType;
	}
}