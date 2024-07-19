#pragma once
#include "../ScriptDefines.h"
#include <memory>
#include <any>
#include <vector>
#include <unordered_map>
#include "../Node/NodeRef.h"

namespace SCR
{

	class ScriptFoundation;
	class ScriptManager;
	class Script;
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

		static MemoryArena<NodeBufferCapacity>& GetVariableMemoryArena(Script& aScript);
		static const MemoryArena<NodeBufferCapacity>& GetVariableMemoryArena(const Script& aScript);

		static NodeGraph& GetEventGraph(Script& aScript);
		static const NodeGraph& GetEventGraph(const Script& aScript);

		static NodeExecutor& GetNodeExecutor();

		static VariableManager& GetVariableManager(Script& aScript);
		static const VariableManager& GetVariableManager(const Script& aScript);
		
		static const std::vector<Variable>& GetVariables(const Script& aScript);
		static const std::vector<Pin>& GetPins(const NodeGraph& aNodeGraph);

		static const Node& GetNode(const NodeGraph& aNodeGraph, NodeID aNodeID);
		static const Pin& GetPin(const NodeGraph& aNodeGraph, PinID aPinID);
		static const Variable& GetVariable(const Script& aScript, const VarID aVarID);
		
	public:

		static std::vector<Node>& GetNodes(NodeGraph& aNodeGraph);
		static std::vector<Variable>& GetVariablesRef(Script& aScript);

		static std::vector<Pin>& GetPins(NodeGraph& aNodeGraph);

		static Node& GetNodeRef(NodeGraph& aNodeGraph, const NodeID aNodeID);
		static Pin& GetPinRef(NodeGraph& aNodeGraph, const PinID aPinID);
		static Variable& GetVariableRef(Script& aScript, const VarID aVarID);

		static std::unordered_map<NodeRef, VarID, NodeRefHasher>& GetNodeRefToVarIDMap(Script& aScript);

	};
}