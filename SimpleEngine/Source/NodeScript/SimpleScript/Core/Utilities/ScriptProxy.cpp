#include "ScriptProxy.h"
#include "../ScriptFoundation.h"
#include "../Script.h"
#include "../ScriptManager.h"
#include "../ScriptInternalModifier.h"
#include "../Node/NodeManager.h"
#include "../Pin/PinManager.h"

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

	CallStack& ScriptProxy::GetCallStack(Script& aScript)
	{
		return aScript.myScriptManager.myFoundation.myCallStack;
	}

	const CallStack& ScriptProxy::GetCallStack(const Script& aScript)
	{
		return aScript.myScriptManager.myFoundation.myCallStack;
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

	/*MemoryPool& ScriptProxy::GetGraphMemoryPool(NodeGraph& aNodeGraph)
	{
		return aNodeGraph.myMemoryPool;
	}

	const MemoryPool& ScriptProxy::GetGraphMemoryPool(const NodeGraph& aNodeGraph)
	{
		return aNodeGraph.myMemoryPool;
	}*/

	MemoryManager& ScriptProxy::GetNodeGraphMemoryManager(NodeGraph& aNodeGraph)
	{
		return aNodeGraph.myMemoryManager;
	}

	/*MemoryPool& ScriptProxy::GetVariableMemoryPool(Script& aScript)
	{
		return *aScript.myVariableManager.myMemoryPool;
	}*/

	MemoryManager& ScriptProxy::GetVariableMemoryManager(Script& aScript)
	{
		return *aScript.myVariableManager.myMemoryManager;
	}

	const NodeManager& ScriptProxy::GetNodeManager(const Script& aScript)
	{
		return *aScript.myEventGraph.myNodeManager;
	}

	NodeGraph& ScriptProxy::GetEventGraph(Script& aScript)
	{
		return aScript.myEventGraph;
	}

	const NodeGraph& ScriptProxy::GetEventGraph(const Script& aScript)
	{
		return aScript.myEventGraph;
	}

	NodeExecutor& ScriptProxy::GetNodeExecutor(Script& aScript)
	{
		return *aScript.myScriptManager.myFoundation.myNodeExecutor;
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
		return *aScript.myCommandTracker;
	}

	const CommandTracker& ScriptProxy::GetCommandTracker(const Script& aScript)
	{
		return *aScript.myCommandTracker;
	}

	const std::vector<Variable>& ScriptProxy::GetVariables(const Script& aScript)
	{
		return aScript.myVariableManager.myVariables;
	}

	const std::vector<Pin>& ScriptProxy::GetPins(const NodeGraph& aNodeGraph)
	{
		return aNodeGraph.myPinManager->myPins;
	}

	const Node& ScriptProxy::GetNode(const NodeGraph& aNodeGraph, NodeID aNodeID)
	{
		return aNodeGraph.myNodeManager->myNodes[aNodeID];
	}

	const Pin& ScriptProxy::GetPin(const NodeGraph& aNodeGraph, PinID aPinID)
	{
		return aNodeGraph.myPinManager->myPins[aPinID];
	}

	const Variable& ScriptProxy::GetVariable(const Script& aScript, const VarID aVarID)
	{
		return aScript.myVariableManager.myVariables[aVarID];
	}

	const std::vector<NodeID>& ScriptProxy::GetNodeIDsByNodeType(const Script& aScript, const NodeTypeID aNodeTypeID)
	{
		return aScript.myEventGraph.myNodeManager->myNodeIDsByNodeTypeID[aNodeTypeID];
	}

	const std::vector<NodeID>& ScriptProxy::GetNodeIDsByNodeType(const NodeGraph& aNodeGraph, const NodeTypeID aNodeTypeID)
	{
		return aNodeGraph.myNodeManager->myNodeIDsByNodeTypeID[aNodeTypeID];
	}

	std::vector<std::vector<NodeID>>& ScriptProxy::GetNodeIDsByNodeTypeContainer(NodeGraph& aNodeGraph)
	{
		return aNodeGraph.myNodeManager->myNodeIDsByNodeTypeID;
	}

	std::vector<Node>& ScriptProxy::GetNodes(NodeGraph& aNodeGraph)
	{
		return aNodeGraph.myNodeManager->myNodes;
	}

	std::vector<Variable>& ScriptProxy::GetVariablesRef(Script& aScript)
	{
		return aScript.myVariableManager.myVariables;
	}

	std::vector<Pin>& ScriptProxy::GetPins(NodeGraph& aNodeGraph)
	{
		return aNodeGraph.myPinManager->myPins;
	}

	Node& ScriptProxy::GetNodeRef(NodeGraph& aNodeGraph, NodeID aNodeID)
	{
		return aNodeGraph.myNodeManager->myNodes[aNodeID];
	}

	Pin& ScriptProxy::GetPinRef(NodeGraph& aNodeGraph, const PinID aPinID)
	{
		return aNodeGraph.myPinManager->myPins[aPinID];
	}

	Variable& ScriptProxy::GetVariableRef(Script& aScript, const VarID aVarID)
	{
		return aScript.myVariableManager.myVariables[aVarID];
	}

	std::unordered_map<NodeID, std::any>& ScriptProxy::GetNodeStateMap(NodeGraph& aNodeGraph)
	{
		return aNodeGraph.myNodeManager->myInternalNodeData;
	}

	std::unordered_map<NodeID, VarID>& ScriptProxy::GetNodeIDToVarIDMap(Script& aScript)
	{
		return aScript.myVariableManager.myNodeIDToVarID;
	}

	CopyBuffer& ScriptProxy::GetGlobalCopyBufferRef(Script& aScript)
	{
		return *aScript.myScriptManager.myFoundation.myCopyBuffer;
	}
}
