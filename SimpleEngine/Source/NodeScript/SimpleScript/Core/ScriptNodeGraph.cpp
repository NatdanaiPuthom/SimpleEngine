#include "ScriptNodeGraph.hpp"
#include "Node/NodeManager.hpp"
#include "Pin/PinManager.hpp"
#include "Global/ScriptGlobal.hpp"
#include "Node/NodeTypeManager.hpp"

namespace SCR
{

	NodeGraph::NodeGraph(const eNodeGraphType aType)
		: mNodeManager(std::make_unique<NodeManager>())
		, mPinManager(std::make_unique<PinManager>())
		, mType(aType)
	{
	}

	NodeGraph::~NodeGraph()
	{
		for (NodeID nodeID = 0; nodeID < mNodeManager->mNodes.size(); ++nodeID)
		{
			const Node& node = mNodeManager->mNodes[nodeID];
			NodeType& nodeType = Global::GetNodeTypeManager().GetNodeType(node.mTypeID);
			std::erase(nodeType.mNodeRefs, NodeRef{ .mNodeID = nodeID, .mNodeGraph = this });
		}
	}

	NodeGraph::NodeGraph(const NodeGraph& aOther)
		: mNodeManager(std::make_unique<NodeManager>(*aOther.mNodeManager))
		, mPinManager(std::make_unique<PinManager>(*aOther.mPinManager))
		, mMemoryArena(aOther.mMemoryArena)
		, mType(aOther.mType)
	{

		for (Pin& pin : mPinManager->myPins)
		{
			pin.mDataPtr = mMemoryArena.GetRenewedPointer(pin.mDataPtr, aOther.mMemoryArena);
		}
	}
	eNodeGraphType NodeGraph::GetType() const
	{
		return mType;
	}
}