#include "NodeExecutor.hpp"
#include "NodeManager.hpp"
#include "../FlyClass.hpp"
#include "NodeTypeManager.hpp"
#include "../ScriptInternalModifier.hpp"
#include "ScriptUtilities.hpp"
#include "FlyClassInstance.hpp"
#include "../Global/ScriptGlobal.hpp"

namespace FLY_NAMESPACE
{

	NodeExecutor::NodeExecutor()
	{
	}

	NodeExecutor::~NodeExecutor()
	{
	}

	void NodeExecutor::ExecuteEvent(const EventID anEventID, ClassInstance& aClassInstance, void* const aOwner, const ExecutionContextBase& anExecutionContext, const bool aExecuteAutoTickers)
	{
		mExecutionContext.mClass = aClassInstance.mClass;
		mExecutionContext.mExecutionContext = &anExecutionContext;
		mExecutionContext.mClassInstance = &aClassInstance;
		mExecutionContext.mNodeGraphInstance = &aClassInstance.mEventGraphInstance;
		mExecutionContext.mOwner = aOwner;

#ifdef FLY_DEBUG
		if (aOwner == nullptr)
		{
			assert(aClassInstance.mClass->GetTargetID() == GetDataTypeID<None>());
		}
#endif

		EventGraph& eventGraph = aClassInstance.mClass->GetEventGraph();
		auto it = eventGraph.myEventNodes.find(anEventID);

		if (it != eventGraph.myEventNodes.end())
		{
			for (NodeID mNodeID : it->second)
			{
				ExecuteNode(NodeExecutionData{ NodeRef{.mNodeID = mNodeID, .mNodeGraph = &eventGraph }, eNodeTriggerReason::Event });
			}
		}

		if (aExecuteAutoTickers)
		{
			for (const NodeExecutionData& executionData : mAutoTickNodes)
			{
				ExecuteNode(executionData);
			}
		}
	}


	void NodeExecutor::ExecuteNode(const NodeExecutionData& aNodeExecutionData)
	{
		const Node& node = ScriptProxy::GetNode(*aNodeExecutionData.mNodeRef.mNodeGraph, aNodeExecutionData.mNodeRef.mNodeID);
		const NodeType& nodeType = Global::GetNodeTypeManager().GetNodeType(node.mTypeID);
		nodeType.mNodeRecipe.mExecuteFunction(aNodeExecutionData, mExecutionContext);
	}

	void NodeExecutor::RegisterAutoTickNode(const NodeRef& aNodeRef)
	{
		mAutoTickNodes.insert({ aNodeRef, eNodeTriggerReason::Event });
	}

	void NodeExecutor::UnregisterAutoTickNode(const NodeRef& aNodeRef)
	{
		mAutoTickNodes.erase({ aNodeRef, eNodeTriggerReason::Event });
	}

}

