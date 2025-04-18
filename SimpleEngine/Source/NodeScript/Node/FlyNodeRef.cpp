#include "FlyNodeRef.hpp"

namespace FLY_NAMESPACE
{

#ifdef FLY_DEBUG
	size_t NodeRefHasher::operator()(const NodeRef& aNodeRef) const
	{
		return reinterpret_cast<size_t>(&aNodeRef.GetNodeGraph()) + static_cast<size_t>(aNodeRef.GetNodeID());

	}
#endif

	NodeRef CreateContextualNodeRef(NodeID aNodeID, NodeGraph& aNodeGraph)
	{
		return NodeRef(aNodeID, aNodeGraph);
	}

	GlobalNodeRef CreateGlobalNodeRef(const NodeRef& aNodeRef, Class& aClass)
	{
		return GlobalNodeRef(aNodeRef.GetNodeID(), aNodeRef.GetNodeGraph(), aClass);
	}

	GlobalNodeRef CreateGlobalNodeRef(NodeID aNodeID, NodeGraph& aNodeGraph, Class& aClass)
	{
		return GlobalNodeRef(aNodeID, aNodeGraph, aClass);
	}

	GlobalNodeRefConst CreateGlobalNodeRef(NodeID aNodeID, const NodeGraph& aNodeGraph, const Class& aClass)
	{
		return GlobalNodeRefConst(aNodeID, aNodeGraph, aClass);
	}
}
