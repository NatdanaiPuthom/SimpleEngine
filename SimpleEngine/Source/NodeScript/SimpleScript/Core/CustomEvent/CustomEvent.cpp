#include "CustomEvent.h"
#include "../Node/NodeTypeRegistry.h"
namespace SCR
{

	static void CustomEventCallerNode(const InternalExecutionContext* aContext)
	{
		// Sets the values of the custom 


		const NodeID callerNodeID = aContext->GetNodeData().nodeRef.nodeID;

		const Node& callerNode = ScriptProxy::GetNode(*aContext->nodeData.nodeRef.nodeGraph, callerNodeID);
		const CustomEventID customEventID = NodeTypeManager::GetInstance().GetCustomEventID(callerNode.typeID);

		const CustomEvent& customEvent = NodeTypeManager::GetInstance().GetCustomEvent(customEventID);

		const NodeType& executorNodeType = Global::GetNodeTypeManager().GetNodeType(customEvent.GetExecutorTypeID());
		const std::vector<NodeRef>& executorNodeRefs = executorNodeType.nodeRefs;

		for (const NodeRef& executorNodeRef : executorNodeRefs)
		{
			aContext->executionQueue->Push(NodeExecutionData{ executorNodeRef, eNodeTriggerReason::Flow });

			const Node& executorNode = ScriptProxy::GetNode(*executorNodeRef.nodeGraph, executorNodeRef.nodeID);

			CopyPinData(*aContext, executorNode.outputPins, callerNode.inputPins, *executorNodeRef.nodeGraph, *aContext->nodeData.nodeRef.nodeGraph, 1);

		}
	}


	static Flow CustomEventExecutorNode()
	{
		return Flow(true);
	}

	CustomEvent::CustomEvent(std::string_view aName)
	{
		myCallerTypeID = RegisterSystemNodeType<eNodeTrait::CustomEvent | eNodeTrait::HasImplicitFlow>(CustomEventCallerNode, "CustomEvent/Call " + std::string(aName), NodeCreationData{ /*.hasImplicitFlow = true*/ });
		myExecutorTypeID = RegisterSystemNodeType<eNodeTrait::CustomEvent>(CustomEventExecutorNode, "CustomEvent/" + std::string(aName));
	}

	CustomEvent::~CustomEvent()
	{
	}
}