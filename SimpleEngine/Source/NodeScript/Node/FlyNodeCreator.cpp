#include "FlyNodeCreator.hpp"

namespace FLY_NAMESPACE
{

	/*PinSetFunction CreatePinSetFunction()
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
								aContext.mExecutionQueue->Push({ CreateContextualNodeRef(connectedInputPin.mNodeID, *aPinSetData.mNodeGraph), eNodeTriggerReason::Flow});
							}
						}
					}
				}
			};
	}*/

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

			outputPinType.mSetPinValueFunction(SetPinValueData
				{ 
				.mNodeGraph = &aDestinationNodeGraph, 
				.mReadFromDataPtr = sourcePin.mDataPtr, 
				.mWriteToPinID = destinationPinID,
#ifdef FLY_DEBUG
				.mReadFromDataTypeID = sourcePinType.mDataTypeID
#endif
				}, aContext);
		}
	}

}
