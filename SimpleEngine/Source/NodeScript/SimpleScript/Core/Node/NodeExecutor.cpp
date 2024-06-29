#include "NodeExecutor.h"
#include "NodeManager.h"
#include "../Script.h"
#include "NodeTypeManager.h"
#include "../ScriptInternalModifier.h"
#include "ScriptUtilities.h"
#include "ScriptInstance.h"

namespace SCR
{

	NodeExecutor::NodeExecutor()
	{
	}

	NodeExecutor::~NodeExecutor()
	{
	}

	void NodeExecutor::ExecuteEvent(const EventID anEventID, ScriptInstance& aScriptInstance, void* const aOwner, const ExecutionContextBase& anExecutionContext, const bool aExecuteAutoTickers)
	{
		myExecutionContext.script = aScriptInstance.myScript;
		myExecutionContext.executionContext = &anExecutionContext;
		myExecutionContext.scriptInstance = &aScriptInstance;
		myExecutionContext.nodeGraphInstance = &aScriptInstance.myEventGraphInstance;
		myExecutionContext.owner = aOwner;

#ifdef FLY_DEBUG
		if (aOwner == nullptr)
		{
			assert(aScriptInstance.myScript->GetTargetID() == GlobalDataTypeID);
		}
#endif

		EventGraph& eventGraph = aScriptInstance.myScript->GetEventGraph();
		auto it = eventGraph.myEventNodes.find(anEventID);

		if (it != eventGraph.myEventNodes.end())
		{
			for (NodeID nodeID : it->second)
			{
				ExecuteNode(NodeExecutionData{ NodeRef{.nodeID = nodeID, .nodeGraph = &eventGraph }, eNodeTriggerReason::Event });
			}
		}

		if (aExecuteAutoTickers)
		{
			for (const NodeExecutionData& executionData : myAutoTickNodes)
			{
				ExecuteNode(executionData);
			}
		}
	}


	void NodeExecutor::ExecuteNode(const NodeExecutionData& aNodeExecutionData)
	{
		const Node& node = ScriptProxy::GetNode(*aNodeExecutionData.nodeRef.nodeGraph, aNodeExecutionData.nodeRef.nodeID);
		const NodeType& nodeType = NodeTypeManager::GetInstance().GetNodeType(node.typeID);
		nodeType.nodeRecipe.executeFunction(aNodeExecutionData, myExecutionContext);
	}

	/*void NodeExecutor::BindToEvent(const NodeRef& aNodeRef, const size_t aTrait)
	{
		if (aTrait != EnumCast(eNodeExecutionTrait::None))
		{
			bool alreadyExists = false;
			for (const NodeExecutionData& nodeExecutionData : myEventNodes[aTrait])
			{
				if (nodeExecutionData.nodeRef == aNodeRef)
				{
					alreadyExists = true;
					break;
				}
			}

			if (!alreadyExists)
			{
				myEventNodes[aTrait].push_back({ aNodeRef, eNodeTriggerReason::Event });
			}
		}
	}

	void NodeExecutor::BindToEvent(const NodeRef& aNodeRef)
	{
		const Node& node = ScriptProxy::GetNode(*aNodeRef.nodeGraph, aNodeRef.nodeID);

		const NodeType& nodeType = NodeTypeManager::GetNodeType(node.typeID);

		eNodeExecutionTrait executionTrait = nodeType.nodeRecipe.executionTrait;

		if (executionTrait != eNodeExecutionTrait::None)
		{
			BindToEvent(aNodeRef, EnumCast(executionTrait));

		}
	}

	void NodeExecutor::UnbindFromEvent(const NodeRef& aNodeRef, const size_t anEventHash)
	{
		if (anEventHash != EnumCast(eNodeExecutionTrait::None))
		{
			std::erase(myEventNodes.at(anEventHash), NodeExecutionData{ aNodeRef });
		}
	}

	void NodeExecutor::UnbindFromEvent(const NodeRef& aNodeRef)
	{
		const Node& node = ScriptProxy::GetNode(*aNodeRef.nodeGraph, aNodeRef.nodeID);

		const NodeType& nodeType = NodeTypeManager::GetNodeType(node.typeID);

		UnbindFromEvent(aNodeRef, EnumCast(nodeType.nodeRecipe.executionTrait));
	}*/

	void NodeExecutor::RegisterAutoTickNode(const NodeRef& aNodeRef)
	{
		myAutoTickNodes.insert({ aNodeRef, eNodeTriggerReason::Event });
	}

	void NodeExecutor::UnregisterAutoTickNode(const NodeRef& aNodeRef)
	{
		myAutoTickNodes.erase({ aNodeRef, eNodeTriggerReason::Event });
	}

}

