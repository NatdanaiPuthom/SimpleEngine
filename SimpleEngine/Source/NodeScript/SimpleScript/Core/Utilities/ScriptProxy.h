#pragma once
#include "../ScriptDefines.h"
#include <memory>
#include <any>
#include <vector>
#include <unordered_map>

namespace SCR
{

	class ScriptFoundation;
	class ScriptManager;
	class Script;
	class NodeTypeManager;
	class PinTypeManager;
	class DataTypeManager;
	class MemoryPool;
	class MemoryManager;
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

		static MemoryManager& GetNodeGraphMemoryManager(NodeGraph& aNodeGraph);

		static MemoryManager& GetVariableMemoryManager(Script& aScript);
		static const MemoryManager& GetVariableMemoryManager(const Script& aScript);

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

		static const std::vector<NodeID>& GetNodeIDsByNodeType(const NodeGraph& aNodeGraph, const NodeTypeID aNodeTypeID);
		static std::vector<std::vector<NodeID>>& GetNodeIDsByNodeTypeContainer(NodeGraph& aNodeGraph);

#pragma region Types
		
	public:

		static std::vector<Node>& GetNodes(NodeGraph& aNodeGraph);
		static std::vector<Variable>& GetVariablesRef(Script& aScript);

		static std::vector<Pin>& GetPins(NodeGraph& aNodeGraph);

		static Node& GetNodeRef(NodeGraph& aNodeGraph, const NodeID aNodeID);
		static Pin& GetPinRef(NodeGraph& aNodeGraph, const PinID aPinID);
		static Variable& GetVariableRef(Script& aScript, const VarID aVarID);



		static std::unordered_map<NodeID, VarID>& GetNodeIDToVarIDMap(Script& aScript);

		static CopyBuffer& GetGlobalCopyBufferRef();

	};
}