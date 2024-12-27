#include "FlyNodeExecutor.hpp"
#include "../DataType/FlyClass.hpp"
#include "FlyNodeTypeManager.hpp"
#include "FlyClassInstance.hpp"
#include "../Internal/FlyInternal.hpp"

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
		const NodeType& nodeType = Internal::GetNodeTypeManager().GetNodeType(node.mTypeID);
		nodeType.mNodeRecipe.mExecuteFunction(aNodeExecutionData, mExecutionContext);
	}

	void NodeExecutor::ExecuteEvent(const EventID anEventID, ClassInstance& aClassInstance, void* const aTarget, const ExecutionContextBase& anExecutionContext)
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
			assert(c.mTargetID == GetDataTypeID<None>());
		}
#endif

		EventGraph& eventGraph = c.mEventGraph;
		auto it = eventGraph.mEventNodes.find(anEventID);

		if (it != eventGraph.mEventNodes.end())
		{
			for (const NodeID nodeID : it->second)
			{
				ExecuteNode(NodeExecutionData{ .mNodeRef = NodeRef(nodeID, eventGraph.mNodeGraph), .mTriggerReason = eNodeTriggerReason::Event });
			}
		}
	}

	Debugger& NodeExecutor::GetDebugger()
	{
		return mDebugger;
	}

}

