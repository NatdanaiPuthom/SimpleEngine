#include "NodeCreator.h"

namespace SCR
{

	PinSetFunction CreatePinSetFunction()
	{
		return [](const PinSetData& aPinSetData, const InternalExecutionContext& aContext) -> void
			{
				const Pin& pin = ScriptProxy::GetPin(*aContext.nodeData.nodeRef.nodeGraph, aPinSetData.id);
				const PinType& pinType = PinTypeManager::GetPinType(pin.typeID);
#ifdef FLY_DEBUG
				assert(aPinSetData.dataTypeID == pinType.dataTypeID);
#endif

				//void* destination = ScriptProxy::GetGraphMemoryPool(*aContext.nodeData.nodeRef.nodeGraph).MemoryAt(pin.memoryID);

				void* destination = pin.dataPtr;

				DataTypeManager::CopyData(pinType.dataTypeID, destination, aPinSetData.value);

				if (pinType.dataTypeID == Flow::typeID)
				{
					const Flow& flow = *reinterpret_cast<const Flow*>(aPinSetData.value);

					if (flow)
					{
						if (pinType.flowType == ePinFlowType::Output)
						{
							assert(aContext.executionQueue);
							for (PinID connectedInputPinID : pin.connectedPinIDs)
							{
								const Pin& connectedInputPin = ScriptProxy::GetPin(*aContext.nodeData.nodeRef.nodeGraph, connectedInputPinID);
								//ScriptProxy::GetNodeExecutor(*aContext.script).Push({ NodeRef{ connectedInputPin.nodeID, aContext.nodeData.nodeRef.nodeGraph }, eNodeTriggerReason::Flow });
								aContext.executionQueue->Push({ NodeRef{ connectedInputPin.nodeID, aContext.nodeData.nodeRef.nodeGraph }, eNodeTriggerReason::Flow });
							}
						}
					}
				}
			};
	}

	void CopyPinData(const InternalExecutionContext& aContext, const std::vector<PinID>& aDestination, const std::vector<PinID>& aSource, const size_t aStartIndex)
	{
		assert(aDestination.size() == aSource.size());
		//const MemoryPool& memoryPool = ScriptProxy::GetGraphMemoryPool(*aContext.nodeData.nodeRef.nodeGraph);
		for (size_t i = aStartIndex; i < aDestination.size(); i++)
		{
			PinID destinationPinID = aDestination[i];

			const Pin& destinationPin = ScriptProxy::GetPin(*aContext.nodeData.nodeRef.nodeGraph, destinationPinID);

			const PinType& outputPinType = PinTypeManager::GetPinType(destinationPin.typeID);

			const PinID sourcePinID = aSource[i];
			const Pin& sourcePin = ScriptProxy::GetPin(*aContext.nodeData.nodeRef.nodeGraph, sourcePinID);
			//const void* value = memoryPool.MemoryAt(sourcePin.memoryID);


			outputPinType.setFunction(PinSetData{ destinationPinID, sourcePin.dataPtr,
#ifdef FLY_DEBUG
				PinTypeManager::GetPinType(sourcePin.typeID).dataTypeID
#endif
				}, aContext);
		}
	}


	void EvaluateInputValues(const std::vector<PinID>& aInputPinIDs, const InternalExecutionContext& aContext, const size_t aStartIndex)
	{
		for (size_t i = aStartIndex; i < aInputPinIDs.size(); ++i)
		{

			const PinID inputPinID = aInputPinIDs[i];

			const Pin& inputPin = ScriptProxy::GetPin(*aContext.nodeData.nodeRef.nodeGraph, inputPinID);

			if (!inputPin.connectedPinIDs.empty())
			{

				const PinID connectedOutputPinID = inputPin.connectedPinIDs[0];

				const Pin& connectedOutputPin = ScriptProxy::GetPin(*aContext.nodeData.nodeRef.nodeGraph, connectedOutputPinID);
				const NodeID connectedNodeID = connectedOutputPin.nodeID;

				const Node& connectedNode = ScriptProxy::GetNode(*aContext.nodeData.nodeRef.nodeGraph, connectedNodeID);
				const NodeType& connectedNodeType = NodeTypeManager::GetNodeType(connectedNode.typeID);

				if (!HasFlag(connectedNodeType.nodeRecipe.traits, eNodeTrait::HasFlow))
				{
					ScriptProxy::GetNodeExecutor(*aContext.script).ExecuteNode({ NodeRef{ connectedNodeID, aContext.nodeData.nodeRef.nodeGraph }, eNodeTriggerReason::Read});
				}

				const Pin& pin = ScriptProxy::GetPin(*aContext.nodeData.nodeRef.nodeGraph, inputPinID);
				const PinType& pinType = PinTypeManager::GetPinType(pin.typeID);

				//const MemoryPool& memoryPool = ScriptProxy::GetGraphMemoryPool(*aContext.nodeData.nodeRef.nodeGraph);

				//const void* value = memoryPool.MemoryAt(connectedOutputPin.memoryID);

				pinType.setFunction(PinSetData{ inputPinID, pin.dataPtr,
#ifdef _DEBUG
					PinTypeManager::GetPinType(connectedOutputPin.typeID).dataTypeID
#endif
					}, aContext);
			}
		}
	}

}
