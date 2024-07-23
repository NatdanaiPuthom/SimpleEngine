#include "NodeCreator.hpp"

namespace FLY_NAMESPACE
{

	PinSetFunction CreatePinSetFunction()
	{
		return [](const PinSetData& aPinSetData, const InternalExecutionContext& aContext) -> void
			{
				Pin& pin = aPinSetData.mNodeGraph->mPins[aPinSetData.mID];
				const PinType& pinType = Global::GetPinTypeManager().GetPinType(pin.mTypeID);
#ifdef FLY_DEBUG
				assert(aPinSetData.mDataTypeID == pinType.mDataTypeID);
#endif

				Global::GetDataTypeManager().CopyData(pinType.mDataTypeID, pin.mDataPtr, aPinSetData.mValue);

				if (pinType.mDataTypeID == Flow::mTypeID)
				{
					const Flow& flow = *reinterpret_cast<const Flow*>(aPinSetData.mValue);

					if (flow)
					{
						if (pinType.mFlowType == eFlowType::Output)
						{
							assert(aContext.mExecutionQueue);
							for (const PinID connectedInputPinID : pin.mConnectedPinIDs)
							{
								const Pin& connectedInputPin = aPinSetData.mNodeGraph->mPins[connectedInputPinID];
								aContext.mExecutionQueue->Push({ NodeRef{ connectedInputPin.mNodeID, aPinSetData.mNodeGraph }, eNodeTriggerReason::Flow });
							}
						}
					}
				}
			};
	}

	void CopyPinData(const InternalExecutionContext& aContext, const std::vector<PinID>& aDestination, const std::vector<PinID>& aSource, NodeGraph& aDestinationNodeGraph, const NodeGraph& aSourceNodeGraph, const size_t aStartIndex)
	{
		assert(aDestination.size() == aSource.size());
		for (size_t i = aStartIndex; i < aDestination.size(); i++)
		{
			const PinID destinationPinID = aDestination[i];

			const Pin& destinationPin = aDestinationNodeGraph.mPins[destinationPinID];

			const PinType& outputPinType = Global::GetPinTypeManager().GetPinType(destinationPin.mTypeID);

			const PinID sourcePinID = aSource[i];
			const Pin& sourcePin = aSourceNodeGraph.mPins[sourcePinID];
			[[maybe_unused]] const PinType& sourcePinType = Global::GetPinTypeManager().GetPinType(sourcePin.mTypeID);

			outputPinType.mSetFunction(PinSetData{ destinationPinID, &aDestinationNodeGraph, sourcePin.mDataPtr,
#ifdef FLY_DEBUG
				sourcePinType.mDataTypeID
#endif
				}, aContext);
		}
	}


	void EvaluateInputValues(const std::vector<PinID>& aInputPinIDs, const InternalExecutionContext& aContext, const size_t aStartIndex)
	{
		for (size_t i = aStartIndex; i < aInputPinIDs.size(); ++i)
		{

			const PinID inputPinID = aInputPinIDs[i];

			const Pin& inputPin = aContext.mNodeData.mNodeRef.mNodeGraph->mPins[inputPinID];
			const PinType& inputPinType = Global::GetPinTypeManager().GetPinType(inputPin.mTypeID);

			if (inputPinType.mDataTypeID == Flow::mTypeID)
			{
				continue;
			}

			if (!inputPin.mConnectedPinIDs.empty())
			{

				const PinID connectedOutputPinID = inputPin.mConnectedPinIDs.front();

				const Pin& connectedOutputPin = aContext.mNodeData.mNodeRef.mNodeGraph->mPins[connectedOutputPinID];
				const NodeID connectedNodeID = connectedOutputPin.mNodeID;

				const Node& connectedNode = aContext.mNodeData.mNodeRef.mNodeGraph->mNodes[connectedNodeID];
				const NodeType& connectedNodeType = Global::GetNodeTypeManager().GetNodeType(connectedNode.mTypeID);

				if (!HasFlag(connectedNodeType.mNodeRecipe.mTraits, eNodeTrait::HasFlow))
				{
					Global::GetNodeExecutor().ExecuteNode({ NodeRef{ connectedNodeID, aContext.mNodeData.mNodeRef.mNodeGraph }, eNodeTriggerReason::Read });
				}

				inputPinType.mSetFunction(PinSetData{ inputPinID, aContext.mNodeData.mNodeRef.mNodeGraph, connectedOutputPin.mDataPtr,
#ifdef _DEBUG
					Global::GetPinTypeManager().GetPinType(connectedOutputPin.mTypeID).mDataTypeID
#endif
					}, aContext);
			}
		}
	}

}
