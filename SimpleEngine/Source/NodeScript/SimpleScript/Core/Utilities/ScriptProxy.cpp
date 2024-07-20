#include "ScriptProxy.hpp"
#include "../ScriptFoundation.hpp"
#include "../FlyClass.hpp"
#include "../ScriptInternalModifier.hpp"
#include "../Node/NodeManager.hpp"
#include "../Pin/PinManager.hpp"

namespace SCR
{

	MemoryPool& ScriptProxy::GetGlobalMemoryPool()
	{
		return ScriptFoundation::GetInstance().myMemoryPool;
	}

	MemoryArena<NodeBufferCapacity>& ScriptProxy::GetNodeGraphMemoryArena(NodeGraph& aNodeGraph)
	{
		return aNodeGraph.mMemoryArena;
	}

	MemoryArena<NodeBufferCapacity>& ScriptProxy::GetVariableMemoryArena(Class& aClass)
	{
		return *aClass.mVariableManager.mMemoryArena;
	}

	const MemoryArena<NodeBufferCapacity>& ScriptProxy::GetVariableMemoryArena(const Class& aScript)
	{
		return *aScript.mVariableManager.mMemoryArena;
	}

	NodeGraph& ScriptProxy::GetEventGraph(Class& aScript)
	{
		return aScript.mEventGraph;
	}

	const NodeGraph& ScriptProxy::GetEventGraph(const Class& aScript)
	{
		return aScript.mEventGraph;
	}

	NodeExecutor& ScriptProxy::GetNodeExecutor()
	{
		return *ScriptFoundation::GetInstance().myNodeExecutor;
	}

	VariableManager& ScriptProxy::GetVariableManager(Class& aScript)
	{
		return aScript.mVariableManager;
	}

	const VariableManager& ScriptProxy::GetVariableManager(const Class& aScript)
	{
		return aScript.mVariableManager;
	}


	const std::vector<Variable>& ScriptProxy::GetVariables(const Class& aScript)
	{
		return aScript.mVariableManager.mVariables;
	}

	const std::vector<Pin>& ScriptProxy::GetPins(const NodeGraph& aNodeGraph)
	{
		return aNodeGraph.mPinManager->myPins;
	}

	const Node& ScriptProxy::GetNode(const NodeGraph& aNodeGraph, NodeID aNodeID)
	{
		return aNodeGraph.mNodeManager->mNodes[aNodeID];
	}

	const Pin& ScriptProxy::GetPin(const NodeGraph& aNodeGraph, PinID aPinID)
	{
		return aNodeGraph.mPinManager->myPins[aPinID];
	}

	const Variable& ScriptProxy::GetVariable(const Class& aScript, const VarID aVarID)
	{
		return aScript.mVariableManager.mVariables[aVarID];
	}

	std::vector<Node>& ScriptProxy::GetNodes(NodeGraph& aNodeGraph)
	{
		return aNodeGraph.mNodeManager->mNodes;
	}

	std::vector<Variable>& ScriptProxy::GetVariablesRef(Class& aScript)
	{
		return aScript.mVariableManager.mVariables;
	}

	std::vector<Pin>& ScriptProxy::GetPins(NodeGraph& aNodeGraph)
	{
		return aNodeGraph.mPinManager->myPins;
	}

	Node& ScriptProxy::GetNodeRef(NodeGraph& aNodeGraph, NodeID aNodeID)
	{
		return aNodeGraph.mNodeManager->mNodes[aNodeID];
	}

	Pin& ScriptProxy::GetPinRef(NodeGraph& aNodeGraph, const PinID aPinID)
	{
		return aNodeGraph.mPinManager->myPins[aPinID];
	}

	Variable& ScriptProxy::GetVariableRef(Class& aScript, const VarID aVarID)
	{
		return aScript.mVariableManager.mVariables[aVarID];
	}

	std::unordered_map<NodeRef, VarID, NodeRefHasher>& ScriptProxy::GetNodeRefToVarIDMap(Class& aScript)
	{
		return aScript.mVariableManager.mNodeRefToVarID;
	}
}
