#pragma once
#include "../ScriptDefines.h"
#include <memory>
#include <any>
#include <vector>
#include <unordered_map>

#include "NodeTypeManager.h"

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
		
		/*static ScriptFoundation& GetFoundation(Script& aScript);
		static const ScriptFoundation& GetFoundation(const Script& aScript);

		static CallStack& GetCallStack(Script& aScript);
		static const CallStack& GetCallStack(const Script& aScript);*/

		static MemoryPool& GetGlobalMemoryPool();


		//static MemoryPool& GetGraphMemoryPool(NodeGraph& aNodeGraph);
		//static const MemoryPool& GetGraphMemoryPool(const NodeGraph& aNodeGraph);

		static MemoryManager& GetNodeGraphMemoryManager(NodeGraph& aNodeGraph);

		//static MemoryPool& GetVariableMemoryPool(Script& aScript);
		//static const MemoryPool& GetVariableMemoryPool(const Script& aScript);
		static MemoryManager& GetVariableMemoryManager(Script& aScript);
		static const MemoryManager& GetVariableMemoryManager(const Script& aScript);

		static NodeGraph& GetEventGraph(Script& aScript);
		static const NodeGraph& GetEventGraph(const Script& aScript);

		//static NodeExecutor& GetNodeExecutor(Script& aScript);
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

		static std::unordered_map<NodeID, std::any>& GetNodeStateMap(NodeGraph& aNodeGraph);

		template<typename T>
		static T& GetNodeState(NodeGraph& aNodeGraph, NodeID aNodeID)
		{
			std::any& anyData = GetNodeStateMap(aNodeGraph).at(aNodeID);
			T& data = std::any_cast<T&>(anyData);
			return data;
		}

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