#pragma once
#include "../ScriptDefines.h"

namespace SCR
{
	
	class NodeGraph;


	struct NodeRef final
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
		if (a.nodeGraph == b.nodeGraph)
		{
			return a.nodeID < b.nodeID;
		}
		return a.nodeGraph < b.nodeGraph;
	}

	struct NodeRefHasher final
	{
		size_t operator()(const NodeRef& aNodeRef) const
		{
			return reinterpret_cast<size_t>(aNodeRef.nodeGraph) + static_cast<size_t>(aNodeRef.nodeID);
		}
	};

	/*struct ConstNodeRef
	{
		NodeID nodeID = InvalidID<NodeID>();
		const NodeGraph* nodeGraph = nullptr;
	};

	inline bool operator==(const ConstNodeRef& a, const ConstNodeRef& b)
	{
		return &a.nodeGraph == &b.nodeGraph && a.nodeID == b.nodeID;
	}

	inline bool operator!=(const ConstNodeRef& a, const ConstNodeRef& b)
	{
		return !(a == b);
	}

	inline bool operator<(const ConstNodeRef& a, const ConstNodeRef& b)
	{
		if (a.nodeGraph == b.nodeGraph)
		{
			return a.nodeID < b.nodeID;
		}
		return a.nodeGraph < b.nodeGraph;
	}*/
}