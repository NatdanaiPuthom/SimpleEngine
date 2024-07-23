#pragma once
#include "../FlyDefines.hpp"
#include <memory>
#include <any>
#include <vector>
#include <unordered_map>
#include "../Node/FlyNodeRef.hpp"

namespace FLY_NAMESPACE
{

	class ScriptFoundation;
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
	class VariableManager;
	class CommandTracker;
	class Node;
	struct Pin;
	struct Variable;
	class InternalModifier;
	class NodeGraph;
	class CallStack;
	struct CopyBuffer;
	
	class ScriptProxy
	{
		friend class ScriptModifier;
		friend class InternalModifier;
		ScriptProxy() = delete;
	public:
	

		static MemoryPool& GetGlobalMemoryPool();

		static MemoryArena<NodeBufferCapacity>& GetNodeGraphMemoryArena(NodeGraph& aNodeGraph);

		static MemoryArena<NodeBufferCapacity>& GetVariableMemoryArena(Class& aScript);
		static const MemoryArena<NodeBufferCapacity>& GetVariableMemoryArena(const Class& aScript);

		static NodeExecutor& GetNodeExecutor();

		static VariableManager& GetVariableManager(Class& aScript);
		static const VariableManager& GetVariableManager(const Class& aScript);
		
		static const std::vector<Variable>& GetVariables(const Class& aScript);
		static const std::vector<Pin>& GetPins(const NodeGraph& aNodeGraph);

		static const Node& GetNode(const NodeGraph& aNodeGraph, NodeID aNodeID);
		static const Pin& GetPin(const NodeGraph& aNodeGraph, PinID aPinID);
		static const Variable& GetVariable(const Class& aScript, const VarID aVarID);
		
	public:

		static std::vector<Node>& GetNodes(NodeGraph& aNodeGraph);
		static std::vector<Variable>& GetVariablesRef(Class& aScript);

		static std::vector<Pin>& GetPins(NodeGraph& aNodeGraph);

		static Node& GetNodeRef(NodeGraph& aNodeGraph, const NodeID aNodeID);
		static Pin& GetPinRef(NodeGraph& aNodeGraph, const PinID aPinID);
		static Variable& GetVariableRef(Class& aScript, const VarID aVarID);

		static std::unordered_map<NodeRef, VarID, NodeRefHasher>& GetNodeRefToVarIDMap(Class& aScript);

	};
}