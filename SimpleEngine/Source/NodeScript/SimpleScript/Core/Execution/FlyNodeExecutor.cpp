#include "FlyNodeExecutor.hpp"
#include "../FlyClass.hpp"
#include "FlyNodeTypeManager.hpp"
#include "FlyClassInstance.hpp"
#include "../Global/FlyGlobal.hpp"

namespace FLY_NAMESPACE
{

	NodeExecutor::NodeExecutor()
	{
		mExecutionContext.mNodeExecutor = this;
	}

	NodeExecutor::~NodeExecutor()
	{
	}


	void NodeExecutor::ExecuteNode(const NodeExecutionData& aNodeExecutionData)
	{
		const Node& node = aNodeExecutionData.mNodeRef.GetNodeGraph().mNodes[aNodeExecutionData.mNodeRef.GetNodeID()];
		const NodeType& nodeType = Global::GetNodeTypeManager().GetNodeType(node.mTypeID);
		nodeType.mNodeRecipe.mExecuteFunction(aNodeExecutionData, mExecutionContext);
	}

	void NodeExecutor::RegisterAutoTickNode(const NodeRef& aNodeRef)
	{
		mAutoTickNodes.insert(NodeExecutionData{ .mNodeRef = aNodeRef, .mTriggerReason = eNodeTriggerReason::Event });
	}

	void NodeExecutor::UnregisterAutoTickNode(const NodeRef& aNodeRef)
	{
		mAutoTickNodes.erase(NodeExecutionData{ .mNodeRef = aNodeRef, .mTriggerReason = eNodeTriggerReason::Event });
	}


	void NodeExecutor::ExecuteEventInternal(const EventID anEventID, ClassInstance& aClassInstance, void* const aOwner, const ExecutionContextBase& anExecutionContext, const bool aExecuteAutoTickers)
	{
		mExecutionContext.mClass = aClassInstance.mClass;
		mExecutionContext.mExecutionContext = &anExecutionContext;
		mExecutionContext.mClassInstance = &aClassInstance;
		mExecutionContext.mNodeGraphInstance = &aClassInstance.mEventGraphInstance;
		mExecutionContext.mOwner = aOwner;

#ifdef FLY_DEBUG
		if (aOwner == nullptr)
		{
			assert(aClassInstance.mClass->mTargetID == GetDataTypeID<None>());
		}
#endif

		EventGraph& eventGraph = aClassInstance.mClass->mEventGraph;
		auto it = eventGraph.mEventNodes.find(anEventID);

		if (it != eventGraph.mEventNodes.end())
		{
			for (const NodeID nodeID : it->second)
			{
				ExecuteNode(NodeExecutionData{ .mNodeRef = CreateContextualNodeRef(nodeID, eventGraph.mNodeGraph), .mTriggerReason = eNodeTriggerReason::Event });
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

	bool NodeExecutor::IsSameTarget(const ClassInstance& aClassInstance, DataTypeID aDataTypeID) const
	{
		return aClassInstance.mClass->mTargetID == aDataTypeID;
	}

}

