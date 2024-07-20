#pragma once
#include "../ScriptDefines.hpp"

namespace SCR
{
	
	class NodeGraph;


	struct NodeRef final
	{
		NodeID mNodeID = InvalidID<NodeID>();
		NodeGraph* mNodeGraph = nullptr;
	};

	inline bool operator==(const NodeRef& a, const NodeRef& b)
	{
		return &a.mNodeGraph == &b.mNodeGraph && a.mNodeID == b.mNodeID;
	}

	inline bool operator!=(const NodeRef& a, const NodeRef& b)
	{
		return !(a == b);
	}

	inline bool operator<(const NodeRef& a, const NodeRef& b)
	{
		if (a.mNodeGraph == b.mNodeGraph)
		{
			return a.mNodeID < b.mNodeID;
		}
		return a.mNodeGraph < b.mNodeGraph;
	}

	struct NodeRefHasher final
	{
		size_t operator()(const NodeRef& aNodeRef) const
		{
			return reinterpret_cast<size_t>(aNodeRef.mNodeGraph) + static_cast<size_t>(aNodeRef.mNodeID);
		}
	};
}