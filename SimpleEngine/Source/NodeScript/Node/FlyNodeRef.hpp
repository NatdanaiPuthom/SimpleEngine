#pragma once
#include "../FlyDefines.hpp"

namespace FLY_NAMESPACE
{

	class Class;
	class NodeGraph;

	struct NodeGraphRef final
	{
		NodeGraph* mNodeGraph = nullptr;
	};

	inline bool operator==(const NodeGraphRef& a, const NodeGraphRef& b)
	{
		return a.mNodeGraph == b.mNodeGraph;
	}

	inline bool operator!=(const NodeGraphRef& a, const NodeGraphRef& b)
	{
		return !(a == b);
	}

	inline bool operator<(const NodeGraphRef& a, const NodeGraphRef& b)
	{
		return a.mNodeGraph < b.mNodeGraph;
	}

	struct NodeRef final
	{
		NodeRef() = default;

		NodeRef(NodeID aNodeID, NodeGraph& aNodeGraph)
			: mNodeGraph(&aNodeGraph)
			, mNodeID(aNodeID)

		{
		}
		NodeGraph& GetNodeGraph() const
		{
			return *mNodeGraph;
		}

		NodeID GetNodeID() const
		{
			return mNodeID;
		}

		friend bool operator==(const NodeRef& a, const NodeRef& b);
		friend bool operator<(const NodeRef& a, const NodeRef& b);

		explicit operator bool() const
		{
			return mNodeID != InvalidID<NodeID>();
		}

	private:

		NodeGraph* mNodeGraph = nullptr;
		NodeID mNodeID = InvalidID<NodeID>();
	};


	NodeRef CreateContextualNodeRef(NodeID aNodeID, NodeGraph& aNodeGraph);

	inline bool operator==(const NodeRef& a, const NodeRef& b)
	{
		return a.mNodeGraph == b.mNodeGraph && a.mNodeID == b.mNodeID;
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

#ifdef FLY_DEBUG
		size_t operator()(const NodeRef& aNodeRef) const;
#else 
		size_t operator()(const NodeRef& aNodeRef) const
		{
			return reinterpret_cast<size_t>(&aNodeRef.GetNodeGraph()) + static_cast<size_t>(aNodeRef.GetNodeID());
		}
#endif

	};


	class GlobalNodeRef final
	{
	public:

		GlobalNodeRef() = default;
		GlobalNodeRef(NodeID aNodeID, NodeGraph& aNodeGraph, Class& aClass)
			: mClass(&aClass)
			, mNodeGraph(&aNodeGraph)
			, mNodeID(aNodeID)
		{

		}

		GlobalNodeRef(NodeID aNodeID, NodeGraph& aNodeGraph)
			: mClass(nullptr)
			, mNodeGraph(&aNodeGraph)
			, mNodeID(aNodeID)
		{

		}

		Class& GetClass() const
		{
			return *mClass;
		}

		NodeGraph& GetNodeGraph() const
		{
			return *mNodeGraph;
		}

		NodeID GetNodeID() const
		{
			return mNodeID;
		}

	private:
		Class* mClass = nullptr;
		NodeGraph* mNodeGraph = nullptr;
		NodeID mNodeID = InvalidID<NodeID>();
	};

	GlobalNodeRef CreateGlobalNodeRef(const NodeRef& aNodeRef, Class& aClass);
	GlobalNodeRef CreateGlobalNodeRef(NodeID aNodeID, NodeGraph& aNodeGraph, Class& aClass);

	inline bool operator==(const GlobalNodeRef& a, const GlobalNodeRef& b)
	{
		return &a.GetNodeGraph() == &b.GetNodeGraph() && a.GetNodeID() == b.GetNodeID();
	}

	struct GlobalNodeRefHasher final
	{
		size_t operator()(const GlobalNodeRef& aNodeRef) const
		{
			return reinterpret_cast<size_t>(&aNodeRef.GetNodeGraph()) + static_cast<size_t>(aNodeRef.GetNodeID());
		}

	};

	class GlobalNodeRefConst final
	{
	public:

		GlobalNodeRefConst() = default;
		GlobalNodeRefConst(NodeID aNodeID, const NodeGraph& aNodeGraph, const Class& aClass)
			: mClass(&aClass)
			, mNodeGraph(&aNodeGraph)
			, mNodeID(aNodeID)
		{

		}

		GlobalNodeRefConst(const GlobalNodeRef& aNodeRef)
			: mClass(&aNodeRef.GetClass())
			, mNodeGraph(&aNodeRef.GetNodeGraph())
			, mNodeID(aNodeRef.GetNodeID())
		{

		}

		const Class& GetClass() const
		{
			return *mClass;
		}

		const NodeGraph& GetNodeGraph() const
		{
			return *mNodeGraph;
		}

		NodeID GetNodeID() const
		{
			return mNodeID;
		}

	private:
		const Class* mClass = nullptr;
		const NodeGraph* mNodeGraph = nullptr;
		NodeID mNodeID = InvalidID<NodeID>();
	};

	GlobalNodeRefConst CreateGlobalNodeRef(NodeID aNodeID, const NodeGraph& aNodeGraph, const Class& aClass);

	inline bool operator==(const GlobalNodeRefConst& a, const GlobalNodeRefConst& b)
	{
		return &a.GetNodeGraph() == &b.GetNodeGraph() && a.GetNodeID() == b.GetNodeID();
	}

	struct GlobalNodeRefConstHasher final
	{
		size_t operator()(const GlobalNodeRef& aNodeRef) const
		{
			return reinterpret_cast<size_t>(&aNodeRef.GetNodeGraph()) + static_cast<size_t>(aNodeRef.GetNodeID());
		}

	};


}