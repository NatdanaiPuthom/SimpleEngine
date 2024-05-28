#include "ScriptNodeGraph.h"
#include "Node/NodeManager.h"
#include "Pin/PinManager.h"

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
		, myMemoryPool(aOther.myMemoryPool)
	{
	}
}