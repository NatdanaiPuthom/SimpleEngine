#include "CustomEvent.h"
#include "../Node/NodeTypeRegistry.h"
namespace SCR
{

	static void CustomEventCallerNode(const InternalExecutionContext* aContext)
	{
		// Sets the values of the custom 


		NodeID callerNodeID = aContext->GetNodeData().nodeID;

		const Node& callerNode = ScriptProxy::GetNode(aContext->script, callerNodeID);
		CustomEventID customEventID = NodeTypeManager::GetCustomEventID(callerNode.typeID);

		const CustomEvent& customEvent = NodeTypeManager::GetCustomEvent(customEventID);

		NodeExecutor& nodeExecutor = ScriptProxy::GetNodeExecutor(aContext->script);

		const std::vector<NodeID>& executorNodeIDs = ScriptProxy::GetNodeIDsByNodeType(aContext->script, customEvent.GetExecutorTypeID());
		for (NodeID executorNodeID : executorNodeIDs)
		{

			nodeExecutor.Push({ executorNodeID, eNodeTriggerReason::Flow });

			const Node& executorNode = ScriptProxy::GetNode(aContext->script, executorNodeID);

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