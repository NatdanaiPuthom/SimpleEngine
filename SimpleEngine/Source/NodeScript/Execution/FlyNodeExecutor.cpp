#include "FlyNodeExecutor.hpp"
#include "../DataType/FlyClass.hpp"
#include "FlyNodeTypeManager.hpp"
#include "FlyClassInstance.hpp"
#include "../Internal/FlyInternal.hpp"
#include "FlyNodeExecutionQueue.hpp"

namespace FLY_NAMESPACE
{

	NodeExecutor::NodeExecutor()
	{
		mExecutionContext.mNodeExecutor = this;
	}

	void NodeExecutor::Initialize()
	{
		mExecutionContext.mNodeTypeManager = &Internal::GetNodeTypeManager();
		mExecutionContext.mPinTypeManager = &Internal::GetPinTypeManager();
		mExecutionContext.mDataTypeManager = &Internal::GetDataTypeManager();
		mExecutionContext.mTraitManager = &Internal::GetTraitManager();
	}

	void NodeExecutor::ExecuteNode(const NodeExecutionData& aNodeExecutionData)
	{
		const Node& node = aNodeExecutionData.mNodeRef.GetNodeGraph().GetNode(aNodeExecutionData.mNodeRef.GetNodeID());
		const NodeType& nodeType = Internal::GetNodeTypeManager().GetNodeType(node.GetTypeID());
		nodeType.GetExecuteFunction().Invoke(aNodeExecutionData, mExecutionContext);
	}

	void NodeExecutor::ExecuteEvent(const EventID aEventID, ClassInstance& aClassInstance, void* const aTarget, const ExecutionContextBase& anExecutionContext)
	{
		Class& c = Internal::GetClassByID(aClassInstance.mClassID);
		mExecutionContext.mClass = &c;
		mExecutionContext.mExecutionContext = &anExecutionContext;
		mExecutionContext.mClassInstance = &aClassInstance;
		mExecutionContext.mNodeGraphInstance = &aClassInstance.mEventGraphInstance;
		mExecutionContext.mTarget = aTarget;
		mExecutionContext.mNodeGraphVariantHandle = &c.mEventGraph;




#ifdef FLY_DEBUG
		if (aTarget == nullptr)
		{
			assert(c.mTargetID == GetDataTypeID<None*>());
		}
#endif

		EventGraph& eventGraph = c.mEventGraph;
		auto nodeIDs = eventGraph.GetNodeIDsByEventID(aEventID);


		if (!nodeIDs)
		{
			return;
		}


		NodeExecutionQueue nodeExecutionQueue(*this);

		mExecutionContext.mNodeExecutionQueue = &nodeExecutionQueue;

		for (const NodeID nodeID : nodeIDs.value())
		{
			nodeExecutionQueue.Push(NodeExecutionData{ .mNodeRef = NodeRef{ nodeID, eventGraph.GetNodeGraph() },
				.mTriggerReason = eNodeTriggerReason::Event });
		}

		nodeExecutionQueue.Execute();
	}

	Debugger& NodeExecutor::GetDebugger()
	{
		return mDebugger;
	}

}

