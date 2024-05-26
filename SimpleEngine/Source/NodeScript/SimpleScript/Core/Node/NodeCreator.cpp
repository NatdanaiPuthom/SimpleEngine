#include "NodeCreator.h"

namespace SCR
{

	PinSetFunction CreatePinSetFunction()
	{
		return [](const PinSetData& aPinSetData, const InternalExecutionContext& aContext) -> void
			{


				const Pin& pin = ScriptProxy::GetPin(aContext.script, aPinSetData.id);
				const PinType& pinType = PinTypeManager::GetPinType(pin.typeID);
#ifdef _DEBUG
				assert(aPinSetData.dataTypeID == pinType.dataTypeID);
#endif

				void* destination = ScriptProxy::GetScriptMemoryPool(aContext.script).MemoryAt(pin.memoryID);

				DataTypeManager::CopyData(pinType.dataTypeID, destination, aPinSetData.value);

				if (pinType.dataTypeID == Flow::typeID)
				{
					const Flow& flow = *reinterpret_cast<const Flow*>(aPinSetData.value);

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

	void CopyPinData(const InternalExecutionContext& aContext, const std::vector<PinID>& aDestination, const std::vector<PinID>& aSource, const size_t aStartIndex)
	{
		assert(aDestination.size() == aSource.size());
		const MemoryPool& memoryPool = ScriptProxy::GetScriptMemoryPool(aContext.script);
		for (size_t i = aStartIndex; i < aDestination.size(); i++)
		{
			PinID destinationPinID = aDestination[i];

			const Pin& destinationPin = ScriptProxy::GetPin(aContext.script, destinationPinID);

			const PinType& outputPinType = PinTypeManager::GetPinType(destinationPin.typeID);

			const PinID sourcePinID = aSource[i];
			const Pin& sourcePin = ScriptProxy::GetPin(aContext.script, sourcePinID);
			const void* value = memoryPool.MemoryAt(sourcePin.memoryID);


			outputPinType.setFunction(PinSetData{ destinationPinID, value,
#ifdef _DEBUG
				PinTypeManager::GetPinType(sourcePin.typeID).dataTypeID
#endif
				}, aContext);
		}
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

				const MemoryPool& memoryPool = ScriptProxy::GetScriptMemoryPool(aContext.script);

				const void* value = memoryPool.MemoryAt(connectedOutputPin.memoryID);

				pinType.setFunction(PinSetData{ inputPinID, value, 
#ifdef _DEBUG
					PinTypeManager::GetPinType(connectedOutputPin.typeID).dataTypeID
#endif
					}, aContext);
			}
		}
	}

}
