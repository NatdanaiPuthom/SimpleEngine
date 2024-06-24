#include "NodeGraphInstance.h"
#include "../ScriptNodeGraph.h"

namespace SCR
{
	NodeGraphInstance::NodeGraphInstance()
	{
	}

	NodeGraphInstance::~NodeGraphInstance()
	{
	}

	void NodeGraphInstance::Init(const NodeGraph& aNodeGraph)
	{
		myNodeManagerInstance.Init(*aNodeGraph.myNodeManager);
	}
}

