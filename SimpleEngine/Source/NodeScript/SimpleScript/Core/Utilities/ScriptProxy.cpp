#include "ScriptProxy.h"
#include "../ScriptFoundation.h"
#include "../Script.h"
#include "../ScriptManager.h"
#include "../ScriptInternalModifier.h"

namespace SCR
{
	ScriptFoundation& ScriptProxy::GetFoundation(Script& aScript)
	{
		return aScript.myScriptManager.myFoundation;
	}

	const ScriptFoundation& ScriptProxy::GetFoundation(const Script& aScript)
	{
		return aScript.myScriptManager.myFoundation;
	}

	MemoryPool& ScriptProxy::GetGlobalMemoryPool()
	{
		return ScriptFoundation::myGlobalMemoryPool;
	}

	ScriptManager& ScriptProxy::GetScriptManager(Script& aScript)
	{
		return aScript.myScriptManager;
	}

	const ScriptManager& ScriptProxy::GetScriptManager(const Script& aScript)
	{
		return aScript.myScriptManager;
	}

	const std::vector<std::unique_ptr<ScriptManager>>& ScriptProxy::GetScriptManagers(ScriptFoundation& aFoundation)
	{
		return aFoundation.myScriptManagers;
	}

	MemoryPool& ScriptProxy::GetScriptMemoryPool(Script& aScript)
	{
		return aScript.myMemoryPool;
	}

	const MemoryPool& ScriptProxy::GetScriptMemoryPool(const Script& aScript)
	{
		return aScript.myMemoryPool;
	}

	const NodeManager& ScriptProxy::GetNodeManager(const Script& aScript)
	{
		return *aScript.myEventGraph.myNodeManager;
	}

	ScriptGraph& ScriptProxy::GetEventGraph(Script& aScript)
	{
		return aScript.myEventGraph;
	}

	NodeExecutor& ScriptProxy::GetNodeExecutor(Script& aScript)
	{
		return aScript.myExecutor;
	}

	VariableManager& ScriptProxy::GetVariableManager(Script& aScript)
	{
		return aScript.myVariableManager;
	}

	const VariableManager& ScriptProxy::GetVariableManager(const Script& aScript)
	{
		return aScript.myVariableManager;
	}

	CommandTracker& ScriptProxy::GetCommandTracker(Script& aScript)
	{
		return aScript.myCommandTracker;
	}

	const CommandTracker& ScriptProxy::GetCommandTracker(const Script& aScript)
	{
		return aScript.myCommandTracker;
	}

	const std::vector<Variable>& ScriptProxy::GetVariables(const Script& aScript)
	{
		return aScript.myVariableManager.myVariables;
	}

	const std::vector<Pin>& ScriptProxy::GetPins(const Script& aScript)
	{
		return aScript.myEventGraph.myPinManager->myPins;
	}

	const Node& ScriptProxy::GetNode(const Script& aScript, NodeID aNodeID)
	{
		return aScript.myEventGraph.myNodeManager->myNodes[aNodeID];
	}

	const Node& ScriptProxy::GetNode(const ScriptGraph& aScriptGraph, NodeID aNodeID)
	{
		return aScriptGraph.myNodeManager->myNodes[aNodeID];
	}

	const Pin& ScriptProxy::GetPin(const Script& aScript, PinID aPinID)
	{
		return aScript.myEventGraph.myPinManager->myPins[aPinID];
	}

	const Variable& ScriptProxy::GetVariable(const Script& aScript, const VarID aVarID)
	{
		return aScript.myVariableManager.myVariables[aVarID];
	}

	const std::vector<NodeID>& ScriptProxy::GetNodeIDsByNodeType(const Script& aScript, const NodeTypeID aNodeTypeID)
	{
		return aScript.myEventGraph.myNodeManager->myNodeIDsByNodeTypeID[aNodeTypeID];
	}

	std::vector<std::vector<NodeID>>& ScriptProxy::GetNodeIDsByNodeTypeContainer(Script& aScript)
	{
		return aScript.myEventGraph.myNodeManager->myNodeIDsByNodeTypeID;
	}

	ScriptInternalModifier& ScriptProxy::GetInternalModifier(Script& aScript)
	{
		return aScript.GetInternalModifier();
	}

	std::vector<Node>& ScriptProxy::GetNodes(Script& aScript)
	{
		return aScript.myEventGraph.myNodeManager->myNodes;
	}

	std::vector<Variable>& ScriptProxy::GetVariablesRef(Script& aScript)
	{
		return aScript.myVariableManager.myVariables;
	}

	std::vector<Pin>& ScriptProxy::GetPins(Script& aScript)
	{
		return aScript.myEventGraph.myPinManager->myPins;
	}

	Node& ScriptProxy::GetNodeRef(Script& aScript, NodeID aNodeID)
	{
		return aScript.myEventGraph.myNodeManager->myNodes[aNodeID];
	}

	Pin& ScriptProxy::GetPinRef(Script& aScript, const PinID aPinID)
	{
		return aScript.myEventGraph.myPinManager->myPins[aPinID];
	}

	Variable& ScriptProxy::GetVariableRef(Script& aScript, const VarID aVarID)
	{
		return aScript.myVariableManager.myVariables[aVarID];
	}

	std::unordered_map<NodeID, std::any>& ScriptProxy::GetNodeStateMap(Script& aScript)
	{
		return aScript.myEventGraph.myNodeManager->myInternalNodeData;
	}

	std::unordered_map<NodeID, VarID>& ScriptProxy::GetNodeIDToVarIDMap(Script& aScript)
	{
		return aScript.myVariableManager.myNodeIDToVarID;
	}
}
