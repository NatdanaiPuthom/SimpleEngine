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
	}

	void NodeExecutor::ExecuteNode(const NodeExecutionData& aNodeExecutionData)
	{
		const Node& node = aNodeExecutionData.mNodeRef.GetNodeGraph().mNodes[aNodeExecutionData.mNodeRef.GetNodeID()];
		const NodeType& nodeType = Internal::GetNodeTypeManager().GetNodeType(node.mTypeID);
		nodeType.mNodeRecipe.mExecuteFunction(aNodeExecutionData, mExecutionContext);
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
		auto it = eventGraph.mEventNodes.find(aEventID);


		if (it == eventGraph.mEventNodes.end())
		{
			return;
		}


		NodeExecutionQueue nodeExecutionQueue(*this);

		mExecutionContext.mNodeExecutionQueue = &nodeExecutionQueue;

		for (const NodeID nodeID : it->second)
		{
			nodeExecutionQueue.Push(NodeExecutionData{.mNodeRef = NodeRef{nodeID, eventGraph.mNodeGraph},
				.mTriggerReason = eNodeTriggerReason::Event });
		}

		nodeExecutionQueue.Execute();
	}

	Debugger& NodeExecutor::GetDebugger()
	{
		return mDebugger;
	}

}

