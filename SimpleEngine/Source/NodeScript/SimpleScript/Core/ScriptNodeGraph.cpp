#include "ScriptNodeGraph.h"
#include "Node/NodeManager.h"
#include "Pin/PinManager.h"
#include "ScriptProxy.h"

namespace SCR
{

	NodeGraph::NodeGraph(const eNodeGraphType aType)
		: myNodeManager(std::make_unique<NodeManager>())
		, myPinManager(std::make_unique<PinManager>())
		, myType(aType)
	{
	}

	NodeGraph::~NodeGraph()
	{
		for (auto& [nodeID, node, nodeType] : *myNodeManager)
		{
			std::erase(nodeType->nodeRefs, NodeRef{ .nodeID = nodeID, .nodeGraph = this });
		}
	}

	NodeGraph::NodeGraph(const NodeGraph& aOther)
		: myNodeManager(std::make_unique<NodeManager>(*aOther.myNodeManager))
		, myPinManager(std::make_unique<PinManager>(*aOther.myPinManager))
		, myMemoryArena(aOther.myMemoryArena)
		, myType(aOther.myType)
	{

		for (Pin& pin : myPinManager->myPins)
		{
			pin.dataPtr = myMemoryArena.GetRenewedPointer(pin.dataPtr, aOther.myMemoryArena);
		}
	}
	eNodeGraphType NodeGraph::GetType() const
	{
		return myType;
	}
}