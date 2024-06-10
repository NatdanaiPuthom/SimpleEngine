#include "ScriptNodeGraph.h"
#include "Node/NodeManager.h"
#include "Pin/PinManager.h"
#include "ScriptProxy.h"

namespace SCR
{

	NodeGraph::NodeGraph()
		: myNodeManager(std::make_unique<NodeManager>())
		, myPinManager(std::make_unique<PinManager>())
	{
	}

	NodeGraph::~NodeGraph()
	{
	}

	NodeGraph::NodeGraph(const NodeGraph& aOther)
		: myNodeManager(std::make_unique<NodeManager>(*aOther.myNodeManager))
		, myPinManager(std::make_unique<PinManager>(*aOther.myPinManager))
		, myMemoryManager(aOther.myMemoryManager)
	{

		for (Pin& pin : myPinManager->myPins)
		{
			pin.dataPtr = myMemoryManager.GetMemory().GetRenewedPointer(pin.dataPtr, aOther.myMemoryManager.GetMemory());
		}
	}
}