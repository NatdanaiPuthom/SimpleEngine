#include "FlyProxy.hpp"
#include "../FlyFoundation.hpp"
#include "../FlyClass.hpp"

namespace FLY_NAMESPACE
{

	MemoryPool& ScriptProxy::GetGlobalMemoryPool()
	{
		return Foundation::GetInstance().mMemoryPool;
	}

	MemoryArena<NodeBufferCapacity>& ScriptProxy::GetNodeGraphMemoryArena(NodeGraph& aNodeGraph)
	{
		return aNodeGraph.mMemoryArena;
	}

	NodeExecutor& ScriptProxy::GetNodeExecutor()
	{
		return *Foundation::GetInstance().mNodeExecutor;
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
		return aNodeGraph.mPins;
	}

	const Node& ScriptProxy::GetNode(const NodeGraph& aNodeGraph, NodeID aNodeID)
	{
		return aNodeGraph.mNodes[aNodeID];
	}

	const Pin& ScriptProxy::GetPin(const NodeGraph& aNodeGraph, PinID aPinID)
	{
		return aNodeGraph.mPins[aPinID];
	}

	const Variable& ScriptProxy::GetVariable(const Class& aScript, const VarID aVarID)
	{
		return aScript.mVariableManager.mVariables[aVarID];
	}

	std::vector<Node>& ScriptProxy::GetNodes(NodeGraph& aNodeGraph)
	{
		return aNodeGraph.mNodes;
	}

	std::vector<Variable>& ScriptProxy::GetVariablesRef(Class& aScript)
	{
		return aScript.mVariableManager.mVariables;
	}

	std::vector<Pin>& ScriptProxy::GetPins(NodeGraph& aNodeGraph)
	{
		return aNodeGraph.mPins;
	}

	Node& ScriptProxy::GetNodeRef(NodeGraph& aNodeGraph, NodeID aNodeID)
	{
		return aNodeGraph.mNodes[aNodeID];
	}

	Pin& ScriptProxy::GetPinRef(NodeGraph& aNodeGraph, const PinID aPinID)
	{
		return aNodeGraph.mPins[aPinID];
	}
}
