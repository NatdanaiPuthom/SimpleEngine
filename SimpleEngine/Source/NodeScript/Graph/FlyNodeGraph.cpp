#include "FlyNodeGraph.hpp"
#include "../Internal/FlyInternal.hpp"
#include "Node/FlyNodeTypeManager.hpp"

namespace FLY_NAMESPACE
{
	NodeGraph::~NodeGraph()
	{
		NodeTypeManager& nodeTypeManager = Internal::GetNodeTypeManager();
		for (NodeID nodeID{ 0 }; nodeID < mNodes.size(); ++nodeID)
		{
			const Node& node = mNodes[nodeID];
			NodeType& nodeType = nodeTypeManager.GetNodeType(node.mTypeID);
			std::erase(nodeType.mNodeRefs, NodeRef(nodeID, *this));
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

	NodeGraph& NodeGraph::operator=(const NodeGraph& aOther)
	{
		mNodes = aOther.mNodes;
		mPins = aOther.mPins;
		mMemoryArena = aOther.mMemoryArena;

		for (Pin& pin : mPins)
		{
			pin.mDataPtr = mMemoryArena.GetRenewedPointer(pin.mDataPtr, aOther.mMemoryArena);
		}

		return *this;
	}
}