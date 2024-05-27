#include "ScriptGraph.h"
#include "Node/NodeManager.h"
#include "Pin/PinManager.h"

namespace SCR
{

	ScriptGraph::ScriptGraph()
		: myNodeManager(std::make_unique<NodeManager>())
		, myPinManager(std::make_unique<PinManager>())
	{
	}

	ScriptGraph::~ScriptGraph()
	{
	}

}