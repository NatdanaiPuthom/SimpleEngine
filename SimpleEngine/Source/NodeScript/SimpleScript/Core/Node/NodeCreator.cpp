#include "NodeCreator.h"

namespace SCR
{

	PinSetFunction CreatePinSetFunction()
	{
		return [](const PinID aPinID, InternalExecutionContext& aContext, const void* aValue) -> void
			{

				const Pin& pin = ScriptProxy::GetPin(aContext.script, aPinID);
				const PinType& pinType = PinTypeManager::GetPinType(pin.typeID);

				void* destination = ScriptProxy::GetScriptMemoryPool(aContext.script).MemoryAt(pin.memoryID);

				DataTypeManager::CopyData(pinType.dataTypeID, destination, aValue);

				if (pinType.dataTypeID == Flow::typeID)
				{
					const Flow& flow = *reinterpret_cast<const Flow*>(aValue);

					if (flow)
					{
						if (pinType.flowType == ePinFlowType::Output)
						{
							for (PinID connectedInputPinID : pin.connectedPinIDs)
							{
								const Pin& connectedInputPin = ScriptProxy::GetPin(aContext.script, connectedInputPinID);
								ScriptProxy::GetNodeExecutor(aContext.script).Push({ connectedInputPin.nodeID, eNodeTriggerReason::Flow });

							}
						}
					}
				}
			};
	}


	void EvaluateInputValues(const std::vector<PinID>& aInputPinIDs, InternalExecutionContext& aContext, const size_t aStartIndex)
	{
		for (size_t i = aStartIndex; i < aInputPinIDs.size(); ++i)
		{

			const PinID inputPinID = aInputPinIDs[i];

			const Pin& inputPin = ScriptProxy::GetPin(aContext.script, inputPinID);

			if (!inputPin.connectedPinIDs.empty())
			{

				const PinID connectedOutputPinID = inputPin.connectedPinIDs[0];

				const Pin& connectedOutputPin = ScriptProxy::GetPin(aContext.script, connectedOutputPinID);
				const NodeID connectedNodeID = connectedOutputPin.nodeID;

				const Node& connectedNode = ScriptProxy::GetNode(aContext.script, connectedNodeID);
				const NodeType& connectedNodeType = NodeTypeManager::GetNodeType(connectedNode.typeID);

				if (!HasFlag(connectedNodeType.nodeRecipe.traits, eNodeTrait::HasFlow))
				{
					ScriptProxy::GetNodeExecutor(aContext.script).ForceExecuteNode({ connectedNodeID, eNodeTriggerReason::Read });
				}

				const Pin& pin = ScriptProxy::GetPin(aContext.script, inputPinID);
				const PinType& pinType = PinTypeManager::GetPinType(pin.typeID);

				MemoryPool& memoryPool = ScriptProxy::GetScriptMemoryPool(aContext.script);

				const void* value = memoryPool.MemoryAt(connectedOutputPin.memoryID);

				pinType.setFunction(inputPinID, aContext, value);
			}
		}
	}

}
