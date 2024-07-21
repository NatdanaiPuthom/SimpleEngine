#include "CustomEvent.hpp"
#include "../Node/NodeTypeRegistry.hpp"

namespace FLY_NAMESPACE
{

	static void CustomEventCallerNode(const InternalExecutionContext* aContext)
	{
		// Sets the values of the custom 


		const NodeID callerNodeID = aContext->GetNodeData().mNodeRef.mNodeID;

		const Node& callerNode = aContext->mNodeData.mNodeRef.mNodeGraph->mNodes[callerNodeID];
		const CustomEventID customEventID = Global::GetNodeTypeManager().GetCustomEventID(callerNode.mTypeID);

		const CustomEvent& customEvent = Global::GetNodeTypeManager().GetCustomEvent(customEventID);

		const NodeType& executorNodeType = Global::GetNodeTypeManager().GetNodeType(customEvent.GetExecutorTypeID());
		const std::vector<NodeRef>& executorNodeRefs = executorNodeType.mNodeRefs;

		for (const NodeRef& executorNodeRef : executorNodeRefs)
		{
			aContext->mExecutionQueue->Push(NodeExecutionData{ executorNodeRef, eNodeTriggerReason::Flow });

			const Node& executorNode = ScriptProxy::GetNode(*executorNodeRef.mNodeGraph, executorNodeRef.mNodeID);

			CopyPinData(*aContext, executorNode.mOutputPins, callerNode.mInputPins, *executorNodeRef.mNodeGraph, *aContext->mNodeData.mNodeRef.mNodeGraph, 1);

		}
	}


	static Flow CustomEventExecutorNode()
	{
		return Flow(true);
	}

	CustomEvent::CustomEvent(std::string_view aName)
	{
		mCallerTypeID = RegisterSystemNodeType<eNodeTrait::CustomEvent | eNodeTrait::HasImplicitFlow>(CustomEventCallerNode, "CustomEvent/Call " + std::string(aName), NodeCreationData{ /*.hasImplicitFlow = true*/ });
		mExecutorTypeID = RegisterSystemNodeType<eNodeTrait::CustomEvent>(CustomEventExecutorNode, "CustomEvent/" + std::string(aName));
	}

	CustomEvent::~CustomEvent()
	{
	}
}