#include "NodeCreator.hpp"

namespace SCR
{

	PinSetFunction CreatePinSetFunction()
	{
		return [](const PinSetData& aPinSetData, const InternalExecutionContext& aContext) -> void
			{
				const Pin& pin = ScriptProxy::GetPin(*aContext.mNodeData.mNodeRef.mNodeGraph, aPinSetData.mID);
				const PinType& pinType = Global::GetPinTypeManager().GetPinType(pin.mTypeID);
#ifdef FLY_DEBUG
				assert(aPinSetData.mDataTypeID == pinType.mDataTypeID);
#endif

				void* destination = pin.mDataPtr;

				Global::GetDataTypeManager().CopyData(pinType.mDataTypeID, destination, aPinSetData.mValue);

				if (pinType.mDataTypeID == Flow::mTypeID)
				{
					const Flow& flow = *reinterpret_cast<const Flow*>(aPinSetData.mValue);

					if (flow)
					{
						if (pinType.mFlowType == eFlowType::Output)
						{
							assert(aContext.mExecutionQueue);
							for (PinID connectedInputPinID : pin.mConnectedPinIDs)
							{
								const Pin& connectedInputPin = ScriptProxy::GetPin(*aContext.mNodeData.mNodeRef.mNodeGraph, connectedInputPinID);
								aContext.mExecutionQueue->Push({ NodeRef{ connectedInputPin.mNodeID, aContext.mNodeData.mNodeRef.mNodeGraph }, eNodeTriggerReason::Flow });
							}
						}
					}
				}
			};
	}

	void CopyPinData(const InternalExecutionContext& aContext, const std::vector<PinID>& aDestination, const std::vector<PinID>& aSource, const NodeGraph& aDestinationNodeGraph, const NodeGraph& aSourceNodeGraph, const size_t aStartIndex)
	{
		assert(aDestination.size() == aSource.size());
		for (size_t i = aStartIndex; i < aDestination.size(); i++)
		{
			const PinID destinationPinID = aDestination[i];

			const Pin& destinationPin = ScriptProxy::GetPin(aDestinationNodeGraph, destinationPinID);

			const PinType& outputPinType = Global::GetPinTypeManager().GetPinType(destinationPin.mTypeID);

			const PinID sourcePinID = aSource[i];
			const Pin& sourcePin = ScriptProxy::GetPin(aSourceNodeGraph, sourcePinID);


			outputPinType.mSetFunction(PinSetData{ destinationPinID, sourcePin.mDataPtr,
#ifdef FLY_DEBUG
				Global::GetPinTypeManager().GetPinType(sourcePin.mTypeID).mDataTypeID
#endif
				}, aContext);
		}
	}


	void EvaluateInputValues(const std::vector<PinID>& aInputPinIDs, const InternalExecutionContext& aContext, const size_t aStartIndex)
	{
		for (size_t i = aStartIndex; i < aInputPinIDs.size(); ++i)
		{

			const PinID inputPinID = aInputPinIDs[i];

			const Pin& inputPin = ScriptProxy::GetPin(*aContext.mNodeData.mNodeRef.mNodeGraph, inputPinID);
			const PinType& inputPinType = Global::GetPinTypeManager().GetPinType(inputPin.mTypeID);

			if (inputPinType.mDataTypeID == Flow::mTypeID)
			{
				continue;
			}

			if (!inputPin.mConnectedPinIDs.empty())
			{

				const PinID connectedOutputPinID = inputPin.mConnectedPinIDs.front();

				const Pin& connectedOutputPin = ScriptProxy::GetPin(*aContext.mNodeData.mNodeRef.mNodeGraph, connectedOutputPinID);
				const NodeID connectedNodeID = connectedOutputPin.mNodeID;

				const Node& connectedNode = ScriptProxy::GetNode(*aContext.mNodeData.mNodeRef.mNodeGraph, connectedNodeID);
				const NodeType& connectedNodeType = Global::GetNodeTypeManager().GetNodeType(connectedNode.mTypeID);

				if (!HasFlag(connectedNodeType.mNodeRecipe.mTraits, eNodeTrait::HasFlow))
				{
					ScriptProxy::GetNodeExecutor().ExecuteNode({ NodeRef{ connectedNodeID, aContext.mNodeData.mNodeRef.mNodeGraph }, eNodeTriggerReason::Read });
				}

				inputPinType.mSetFunction(PinSetData{ inputPinID, connectedOutputPin.mDataPtr,
#ifdef _DEBUG
					Global::GetPinTypeManager().GetPinType(connectedOutputPin.mTypeID).mDataTypeID
#endif
					}, aContext);
			}
		}
	}

}
