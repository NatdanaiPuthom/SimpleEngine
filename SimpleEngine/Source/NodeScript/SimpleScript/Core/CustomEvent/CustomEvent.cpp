#include "CustomEvent.h"
#include "../Node/NodeTypeRegistry.h"

namespace SCR
{

	static void CustomEventCallerNode(const InternalExecutionContext* aContext)
	{
		// Sets the values of the custom 

		NodeID callerNodeID = aContext->GetNodeData().currentNodeID;

		const Node& callerNode = ScriptProxy::GetNode(aContext->script, callerNodeID);
		CustomEventID customEventID = NodeTypeManager::GetCustomEventID(callerNode.typeID);

		const CustomEvent& customEvent = NodeTypeManager::GetCustomEvent(customEventID);

		const MemoryPool& memoryPool = ScriptProxy::GetScriptMemoryPool(aContext->script);
		NodeExecutor& nodeExecutor = ScriptProxy::GetNodeExecutor(aContext->script);

		const std::vector<NodeID>& executorNodeIDs = ScriptProxy::GetNodeIDsByNodeType(aContext->script, customEvent.myExecutorTypeID);
		for (NodeID executorNodeID : executorNodeIDs)
		{

			nodeExecutor.Push({ executorNodeID, eNodeTriggerReason::Flow });

			const Node& executorNode = ScriptProxy::GetNode(aContext->script, executorNodeID);

			for (size_t i = 1; i < executorNode.outputPins.size(); i++)
			{
				PinID outputPinID = executorNode.outputPins.at(i);

				const Pin& outputPin = ScriptProxy::GetPin(aContext->script, outputPinID);

				const PinType& outputPinType = PinTypeManager::GetPinType(outputPin.typeID);

				const PinID callerInputPinID = callerNode.inputPins.at(i);
				const Pin& callerPin = ScriptProxy::GetPin(aContext->script, callerInputPinID);
				const void* value = memoryPool.MemoryAt(callerPin.memoryID);


				outputPinType.setFunction(outputPinID, *aContext, value);
			}
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