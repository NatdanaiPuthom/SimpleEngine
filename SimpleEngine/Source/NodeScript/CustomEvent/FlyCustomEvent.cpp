#include "FlyCustomEvent.hpp"
#include "../Node/FlyNodeTypeRegistry.hpp"

namespace FLY_NAMESPACE
{

	static void CustomEventCallerNode(InternalExecutionContextPtr aContext)
	{
		// Sets the values of the custom 

		const Node& callerNode = aContext->mNodeData.mNodeRef.GetNodeGraph().GetNode(aContext->mNodeData.mNodeRef.GetNodeID());
		const CustomEventID customEventID = Internal::GetNodeTypeManager().GetCustomEventID(callerNode.GetTypeID());

		const CustomEvent& customEvent = Internal::GetNodeTypeManager().GetCustomEvent(customEventID);

		const NodeType& executorNodeType = Internal::GetNodeTypeManager().GetNodeType(customEvent.GetExecutorTypeID());
		const std::vector<NodeRef>& executorNodeRefs = executorNodeType.GetNodeRefs();

		for (const NodeRef& executorNodeRef : executorNodeRefs)
		{
			aContext->mNodeExecutionQueue->Push(NodeExecutionData{ executorNodeRef, eNodeTriggerReason::Flow });

			const Node& executorNode = executorNodeRef.GetNodeGraph().GetNode(executorNodeRef.GetNodeID());

			CopyPinData(*aContext, executorNode.GetOutputPins(), callerNode.GetInputPins(), executorNodeRef.GetNodeGraph(), aContext->mNodeData.mNodeRef.GetNodeGraph(), 1);

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