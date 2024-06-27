#include "ScriptProxy.h"
#include "../ScriptFoundation.h"
#include "../Script.h"
#include "../ScriptManager.h"
#include "../ScriptInternalModifier.h"
#include "../Node/NodeManager.h"
#include "../Pin/PinManager.h"

namespace SCR
{

	MemoryPool& ScriptProxy::GetGlobalMemoryPool()
	{
		return ScriptFoundation::GetInstance().myMemoryPool;
	}

	MemoryManager& ScriptProxy::GetNodeGraphMemoryManager(NodeGraph& aNodeGraph)
	{
		return aNodeGraph.myMemoryManager;
	}

	MemoryManager& ScriptProxy::GetVariableMemoryManager(Script& aScript)
	{
		return *aScript.myVariableManager.myMemoryManager;
	}

	NodeGraph& ScriptProxy::GetEventGraph(Script& aScript)
	{
		return aScript.myEventGraph;
	}

	const NodeGraph& ScriptProxy::GetEventGraph(const Script& aScript)
	{
		return aScript.myEventGraph;
	}

	NodeExecutor& ScriptProxy::GetNodeExecutor()
	{
		return *ScriptFoundation::GetInstance().myNodeExecutor;
	}

	VariableManager& ScriptProxy::GetVariableManager(Script& aScript)
	{
		return aScript.myVariableManager;
	}

	const VariableManager& ScriptProxy::GetVariableManager(const Script& aScript)
	{
		return aScript.myVariableManager;
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

	std::unordered_map<NodeID, VarID>& ScriptProxy::GetNodeIDToVarIDMap(Script& aScript)
	{
		return aScript.myVariableManager.myNodeIDToVarID;
	}
}
