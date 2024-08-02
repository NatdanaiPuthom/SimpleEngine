#pragma once
#include "../FlyDefines.hpp"
#include <memory>
#include <any>
#include <vector>
#include <unordered_map>
#include "../Node/FlyNodeRef.hpp"

namespace FLY_NAMESPACE
{

	class Foundation;
	class Class;
	class NodeTypeManager;
	class PinTypeManager;
	class DataTypeManager;
	class MemoryPool;
	template<size_t> class MemoryArena;
	class NodeManager;
	class PinManager;
	class NodeExecutor;
	class ScriptModifier;
	class CommandTracker;
	class Node;
	struct Pin;
	struct Variable;
	class InternalModifier;
	class NodeGraph;
	class CallStack;
	
	class ScriptProxy
	{
		friend class ScriptModifier;
		friend class InternalModifier;
		ScriptProxy() = delete;
	public:
	

		static MemoryPool& GetGlobalMemoryPool();

		static MemoryArena<NodeBufferCapacity>& GetNodeGraphMemoryArena(NodeGraph& aNodeGraph);

		static NodeExecutor& GetNodeExecutor();
		
		static const std::vector<Pin>& GetPins(const NodeGraph& aNodeGraph);

		static const Node& GetNode(const NodeGraph& aNodeGraph, NodeID aNodeID);
		static const Pin& GetPin(const NodeGraph& aNodeGraph, PinID aPinID);
		
	public:

		static std::vector<Node>& GetNodes(NodeGraph& aNodeGraph);

		static std::vector<Pin>& GetPins(NodeGraph& aNodeGraph);

		static Node& GetNodeRef(NodeGraph& aNodeGraph, const NodeID aNodeID);
		static Pin& GetPinRef(NodeGraph& aNodeGraph, const PinID aPinID);

	};
}