#include "NodeExecutor.h"
#include "NodeManager.h"
#include "../Script.h"
#include "NodeTypeManager.h"
#include "../ScriptInternalModifier.h"
#include "ReferenceWrapper.h"

namespace SCR
{
	NodeExecutor::NodeExecutor()
	{
	}

	NodeExecutor::~NodeExecutor()
	{
	}

	void NodeExecutor::ExecuteEvent(const eNodeExecutionTrait aTrait, Script& aScript, const ExecutionContextBase& anExecutionContext)
	{
		myExecutionContext.script = &aScript;
		myExecutionContext.executionContext = &anExecutionContext;
		ExecuteInternal(myEventNodes[aTrait]);

		if (aTrait == eNodeExecutionTrait::Tick)
		{
			std::vector<NodeExecutionData> tickNodes;
			tickNodes.reserve(myAutoTickNodes.size());
			for (const NodeExecutionData& executionData : myAutoTickNodes)
			{
				tickNodes.push_back(executionData);
			}
			ExecuteInternal(tickNodes);
		}
	}


	void NodeExecutor::ExecuteNode(const NodeExecutionData& aNodeExecutionData)
	{
		const Node& node = ScriptProxy::GetNode(*aNodeExecutionData.nodeRef.nodeGraph, aNodeExecutionData.nodeRef.nodeID);
		const NodeType& nodeType = NodeTypeManager::GetNodeType(node.typeID);
		nodeType.nodeRecipe.executeFunction(aNodeExecutionData, myExecutionContext);
	}

	void NodeExecutor::BindToEvent(const NodeRef& aNodeRef, const eNodeExecutionTrait aTrait)
	{
		if (aTrait != eNodeExecutionTrait::None)
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
			BindToEvent(aNodeRef, executionTrait);

		}
	}

	void NodeExecutor::UnbindFromEvent(const NodeRef& aNodeRef, const eNodeExecutionTrait aTrait)
	{
		if (aTrait != eNodeExecutionTrait::None)
		{
			std::erase(myEventNodes.at(aTrait), NodeExecutionData{ aNodeRef });
		}
	}

	void NodeExecutor::UnbindFromEvent(const NodeRef& aNodeRef)
	{
		const Node& node = ScriptProxy::GetNode(*aNodeRef.nodeGraph, aNodeRef.nodeID);

		const NodeType& nodeType = NodeTypeManager::GetNodeType(node.typeID);

		UnbindFromEvent(aNodeRef, nodeType.nodeRecipe.executionTrait);
	}

	void NodeExecutor::RegisterAutoTickNode(const NodeRef& aNodeRef)
	{
		myAutoTickNodes.insert({ aNodeRef, eNodeTriggerReason::Event });
	}

	void NodeExecutor::UnregisterAutoTickNode(const NodeRef& aNodeRef)
	{
		myAutoTickNodes.erase({ aNodeRef, eNodeTriggerReason::Event });
	}

	void NodeExecutor::ExecuteInternal(const std::vector<NodeExecutionData>& aNodes)
	{
		for (const NodeExecutionData& node : aNodes)
		{
			ExecuteNode(node);
		}
	}

}

