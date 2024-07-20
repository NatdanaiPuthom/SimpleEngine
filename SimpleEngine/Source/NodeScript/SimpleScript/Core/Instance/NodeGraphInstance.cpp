#include "NodeGraphInstance.hpp"
#include "../ScriptNodeGraph.hpp"

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
		mNodeManagerInstance.Init(*aNodeGraph.mNodeManager);
	}
}

