#include "FlyCustomEvent.hpp"
#include "../Node/FlyNodeTypeRegistry.hpp"

namespace FLY_NAMESPACE
{

	static void CustomEventCallerNode(const InternalExecutionContext* aContext)
	{
		// Sets the values of the custom 

		const Node& callerNode = aContext->mNodeData.mNodeRef.GetNodeGraph().mNodes[aContext->mNodeData.mNodeRef.GetNodeID()];
		const CustomEventID customEventID = Global::GetNodeTypeManager().GetCustomEventID(callerNode.mTypeID);

		const CustomEvent& customEvent = Global::GetNodeTypeManager().GetCustomEvent(customEventID);

		const NodeType& executorNodeType = Global::GetNodeTypeManager().GetNodeType(customEvent.GetExecutorTypeID());
		const std::vector<NodeRef>& executorNodeRefs = executorNodeType.mNodeRefs;

		for (const NodeRef& executorNodeRef : executorNodeRefs)
		{
			aContext->mExecutionQueue->Push(NodeExecutionData{ executorNodeRef, eNodeTriggerReason::Flow });

			const Node& executorNode = executorNodeRef.GetNodeGraph().mNodes[executorNodeRef.GetNodeID()];

			CopyPinData(*aContext, executorNode.mOutputPins, callerNode.mInputPins, executorNodeRef.GetNodeGraph(), aContext->mNodeData.mNodeRef.GetNodeGraph(), 1);

		}
	}


	static Flow CustomEventExecutorNode()
	{
		return Flow(true);
	}

	static NodeCreationData GetCallerNodeCreationData(const std::string_view aCustomEventName)
	{
		NodeCreationData data;
		data.mName = "CustomEvent/Call " + std::string(aCustomEventName);
		return data;
	}

	static NodeCreationData GetExecutorNodeCreationData(const std::string_view aCustomEventName)
	{
		NodeCreationData data;
		data.mName = "CustomEvent/" + std::string(aCustomEventName);
		return data;
	}

	CustomEvent::CustomEvent(std::string_view aName)
	{
		mCallerTypeID = RegisterSystemNodeType<eNodeTrait::CustomEvent | eNodeTrait::HasImplicitFlow>(CustomEventCallerNode, GetCallerNodeCreationData(aName));
		mExecutorTypeID = RegisterSystemNodeType<eNodeTrait::CustomEvent>(CustomEventExecutorNode, GetExecutorNodeCreationData(aName));
	}

	CustomEvent::~CustomEvent()
	{
	}
}