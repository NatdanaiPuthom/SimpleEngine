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
	class NodeManager;
	class PinManager;
	class NodeExecutor;
	class ScriptModifier;
	class VariableManager;
	class CommandTracker;
	class Node;
	struct Pin;
	struct Variable;
	class ScriptInternalModifier;
	class NodeGraph;
	class CallStack;
	
	class ScriptProxy
	{
		friend class ScriptModifier;
		friend class ScriptInternalModifier;
		ScriptProxy() = delete;
	public:
		
		static ScriptFoundation& GetFoundation(Script& aScript);
		static const ScriptFoundation& GetFoundation(const Script& aScript);

		static CallStack& GetCallStack(Script& aScript);
		static const CallStack& GetCallStack(const Script& aScript);

		static MemoryPool& GetGlobalMemoryPool();

		static ScriptManager& GetScriptManager(Script& aScript);
		static const ScriptManager& GetScriptManager(const Script& aScript);

		static const std::vector<std::unique_ptr<ScriptManager>>& GetScriptManagers(ScriptFoundation& aFoundation);

		static MemoryPool& GetScriptMemoryPool(Script& aScript);
		static const MemoryPool& GetScriptMemoryPool(const Script& aScript);

		static const NodeManager& GetNodeManager(const Script& aScript);

		static NodeGraph& GetEventGraph(Script& aScript);


		static NodeExecutor& GetNodeExecutor(Script& aScript);

		static VariableManager& GetVariableManager(Script& aScript);
		static const VariableManager& GetVariableManager(const Script& aScript);

		static CommandTracker& GetCommandTracker(Script& aScript);
		static const CommandTracker& GetCommandTracker(const Script& aScript);
		
		static const std::vector<Variable>& GetVariables(const Script& aScript);
		static const std::vector<Pin>& GetPins(const Script& aScript);

		static const Node& GetNode(const Script& aScript, NodeID aNodeID);
		static const Node& GetNode(const NodeGraph& aNodeGraph, NodeID aNodeID);
		static const Pin& GetPin(const Script& aScript, PinID aPinID);
		static const Variable& GetVariable(const Script& aScript, const VarID aVarID);

		static const std::vector<NodeID>& GetNodeIDsByNodeType(const Script& aScript, const NodeTypeID aNodeTypeID);
		static std::vector<std::vector<NodeID>>& GetNodeIDsByNodeTypeContainer(Script& aScript);
		
	private:

		static ScriptInternalModifier& GetInternalModifier(Script& aScript);

		static std::vector<Node>& GetNodes(Script& aScript);
		static std::vector<Variable>& GetVariablesRef(Script& aScript);

		static std::vector<Pin>& GetPins(Script& aScript);

		static Node& GetNodeRef(Script& aScript, const NodeID aNodeID);
		static Pin& GetPinRef(Script& aScript, const PinID aPinID);
		static Variable& GetVariableRef(Script& aScript, const VarID aVarID);


		static std::unordered_map<NodeID, std::any>& GetNodeStateMap(Script& aScript);

		static std::unordered_map<NodeID, VarID>& GetNodeIDToVarIDMap(Script& aScript);

	};
}