#pragma once
#include "../ScriptDefines.h"

namespace SCR
{
	
	class NodeGraph;


	struct NodeRef
	{
		NodeID nodeID;
		NodeGraph* nodeGraph;
	};
}