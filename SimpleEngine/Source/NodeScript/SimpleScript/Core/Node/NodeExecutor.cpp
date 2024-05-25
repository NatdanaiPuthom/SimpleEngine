#include "NodeExecutor.h"
#include "NodeManager.h"
#include "../Script.h"
#include "NodeTypeManager.h"
#include "../ScriptInternalModifier.h"

namespace SCR
{
	NodeExecutor::NodeExecutor(ScriptInternalModifier& aModifier)
		: myExecutionContext(aModifier.myScript, aModifier)
	{
	}

	NodeExecutor::~NodeExecutor()
	{
	}

	void NodeExecutor::Execute(const std::string& aUserEventKey, const ExecutionContextBase& anExecutionContext)
	{
		//myCurrentNodes = myUserEventNodes[aUserEventName];
		ExecuteInternal(myUserEventNodes[aUserEventKey], anExecutionContext);
	}

	void NodeExecutor::Execute(const eNodeExecutionTrait aTrait, const ExecutionContextBase& anExecutionContext)
	{
		/*myExecutionContext.executionContext = &anExecutionContext;

		myCurrentNodes = myEventNodes[aTrait];
		myFrameNodes.insert(myCurrentNodes.begin(), myCurrentNodes.end());

		auto executeFunc = [this]() -> void
		{
			while (!myCurrentNodes.empty())
			{
				std::vector<NodeExecutionData> currentNodes = myCurrentNodes;

				myCurrentNodes.clear();
				for (const NodeExecutionData& nodeExecutionData : currentNodes)
				{
					ExecuteNode(nodeExecutionData);
				}
			}
		};

		executeFunc();*/

		ExecuteInternal(myEventNodes[aTrait], anExecutionContext);

		if (aTrait == eNodeExecutionTrait::Tick)
		{
			for (const NodeExecutionData& executionData : myAutoTickNodes)
			{
				Push(executionData);
			}
			ExecuteInternal(myCurrentNodes, anExecutionContext);
			//executeFunc();
		}
		//myFrameNodes.clear();
	}

	void NodeExecutor::Push(const NodeExecutionData& aNodeExecutionData)
	{
	/*	if (!myFrameNodes.contains(aNodeExecutionData))
		{*/
			myCurrentNodes.push_back(aNodeExecutionData);
			//myFrameNodes.insert(aNodeExecutionData);
		//}
	}

	void NodeExecutor::ForceExecuteNode(const NodeExecutionData& aNodeExecutionData)
	{
		//myFrameNodes.insert(aNodeExecutionData);
		ExecuteNode(aNodeExecutionData);
	}

	/*void NodeExecutor::TryExecuteNode(const NodeExecutionData aNodeExecutionData)
	{
		if (!myFrameNodes.contains(aNodeExecutionData))
		{
			myFrameNodes.insert(aNodeExecutionData);
			ExecuteNode(aNodeExecutionData);
		}
	}*/

	void NodeExecutor::Register(const NodeID aNodeID, const eNodeExecutionTrait aTrait)
	{
		if (aTrait != eNodeExecutionTrait::None)
		{
			bool alreadyExists = false;
			for (const NodeExecutionData& nodeExecutionData : myEventNodes[aTrait])
			{
				if (nodeExecutionData.currentNodeID == aNodeID)
				{
					alreadyExists = true;
					break;
				}
			}

			if (!alreadyExists)
			{
				myEventNodes[aTrait].push_back({ aNodeID, eNodeTriggerReason::Event });
			}
		}
	}

	void NodeExecutor::Register(const NodeID aNodeID)
	{
		const Node& node = ScriptProxy::GetNode(myExecutionContext.script, aNodeID);

		const NodeType& nodeType = NodeTypeManager::GetNodeType(node.typeID);

		eNodeExecutionTrait executionTrait = nodeType.nodeRecipe.executionTrait;

		if (executionTrait != eNodeExecutionTrait::None)
		{
			Register(aNodeID, executionTrait);

		}
	}

	void NodeExecutor::Unregister(const NodeID aNodeID, const eNodeExecutionTrait aTrait)
	{
		if (aTrait != eNodeExecutionTrait::None)
		{
			std::erase(myEventNodes.at(aTrait), NodeExecutionData{ aNodeID });
		}
	}

	void NodeExecutor::Unregister(const NodeID aNodeID)
	{
		const Node& node = ScriptProxy::GetNode(myExecutionContext.script, aNodeID);

		const NodeType& nodeType = NodeTypeManager::GetNodeType(node.typeID);

		Unregister(aNodeID, nodeType.nodeRecipe.executionTrait);
	}

	void NodeExecutor::RegisterAutoTickNode(const NodeID aNodeID)
	{
		myAutoTickNodes.insert({ aNodeID, eNodeTriggerReason::Event });
	}

	void NodeExecutor::UnregisterAutoTickNode(const NodeID aNodeID)
	{
		myAutoTickNodes.erase({ aNodeID, eNodeTriggerReason::Event });
	}

	void NodeExecutor::ExecuteInternal(const std::vector<NodeExecutionData>& aNodes, const ExecutionContextBase& anExecutionContext)
	{
		myExecutionContext.executionContext = &anExecutionContext;

		myCurrentNodes = aNodes;
		//myFrameNodes.insert(myCurrentNodes.begin(), myCurrentNodes.end());

		while (!myCurrentNodes.empty())
		{
			std::vector<NodeExecutionData> currentNodes = myCurrentNodes;

			myCurrentNodes.clear();
			for (const NodeExecutionData& nodeExecutionData : currentNodes)
			{
				ExecuteNode(nodeExecutionData);
			}
		}

		
		//myFrameNodes.clear();
	}

	void NodeExecutor::ExecuteNode(const NodeExecutionData& aNodeExecutionData)
	{
		const Node& node = ScriptProxy::GetNode(myExecutionContext.script, aNodeExecutionData.currentNodeID);
		const NodeType& nodeType = NodeTypeManager::GetNodeType(node.typeID);
		nodeType.nodeRecipe.executeFunction(aNodeExecutionData, myExecutionContext);
	}

}

