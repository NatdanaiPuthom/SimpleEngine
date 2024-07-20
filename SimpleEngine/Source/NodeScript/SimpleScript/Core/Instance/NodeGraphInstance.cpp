#include "NodeGraphInstance.hpp"
#include "../ScriptNodeGraph.hpp"

namespace FLY_NAMESPACE
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

