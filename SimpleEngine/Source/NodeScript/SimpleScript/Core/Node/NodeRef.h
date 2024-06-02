#pragma once
#include "../ScriptDefines.h"

namespace SCR
{
	
	class NodeGraph;


	struct NodeRef
	{
		NodeID nodeID = InvalidID<NodeID>();
		NodeGraph* nodeGraph = nullptr;

	};



	inline bool operator==(const NodeRef& a, const NodeRef& b)
	{
		return &a.nodeGraph == &b.nodeGraph && a.nodeID == b.nodeID;
	}

	inline bool operator!=(const NodeRef& a, const NodeRef& b)
	{
		return !(a == b);
	}

	inline bool operator<(const NodeRef& a, const NodeRef& b)
	{
		return a.nodeID < b.nodeID;
	}
}