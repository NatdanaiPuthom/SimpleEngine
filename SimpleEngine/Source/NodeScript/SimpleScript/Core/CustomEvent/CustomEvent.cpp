#include "CustomEvent.h"
#include "../Node/NodeTypeRegistry.h"
namespace SCR
{

	static void CustomEventCallerNode(const InternalExecutionContext* aContext)
	{
		// Sets the values of the custom 


		NodeID callerNodeID = aContext->GetNodeData().nodeRef.nodeID;

		const Node& callerNode = ScriptProxy::GetNode(*aContext->nodeData.nodeRef.nodeGraph, callerNodeID);
		CustomEventID customEventID = NodeTypeManager::GetInstance().GetCustomEventID(callerNode.typeID);

		const CustomEvent& customEvent = NodeTypeManager::GetInstance().GetCustomEvent(customEventID);

		const std::vector<NodeID>& executorNodeIDs = ScriptProxy::GetNodeIDsByNodeType(*aContext->nodeData.nodeRef.nodeGraph, customEvent.GetExecutorTypeID());
		for (NodeID executorNodeID : executorNodeIDs)
		{
			aContext->executionQueue->Push({ NodeRef{ executorNodeID, aContext->nodeData.nodeRef.nodeGraph }, eNodeTriggerReason::Flow });

			const Node& executorNode = ScriptProxy::GetNode(*aContext->nodeData.nodeRef.nodeGraph, executorNodeID);

			CopyPinData(*aContext, executorNode.outputPins, callerNode.inputPins, 1);

		}
	}


	static Flow CustomEventExecutorNode()
	{
		return true;
	}

	CustomEvent::CustomEvent(const std::string& aName)
	{
		myCallerTypeID = RegisterSystemNodeType<eNodeTrait::CustomEvent | eNodeTrait::HasImplicitFlow>(CustomEventCallerNode, "CustomEvent/Call " + aName);
		myExecutorTypeID = RegisterSystemNodeType<eNodeTrait::CustomEvent>(CustomEventExecutorNode, "CustomEvent/" + aName);
	}

	CustomEvent::~CustomEvent()
	{
	}
}