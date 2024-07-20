#include "PinView.hpp"
#include "../ScriptNodeGraph.hpp"
#include "../Global/ScriptGlobal.hpp"
#include "../Pin/PinTypeManager.hpp"
#include "../DataType/DataTypeManager.hpp"

namespace FLY_NAMESPACE
{

	PinView::PinView(const PinID aPinID, const NodeGraph& aNodeGraph)
		: myPinID(aPinID)
		, myNodeGraph(&aNodeGraph)
	{
	}
	const std::vector<PinID>& PinView::GetConnectedPinIDs() const
	{
		return GetPin().mConnectedPinIDs;
	}

	DataTypeID PinView::GetDataTypeID() const
	{
		return GetPinType().mDataTypeID;
	}

	const std::string& PinView::GetPinTypeName() const
	{
		const PinType& pinType = GetPinType();
		if (pinType.mName == "#T")
		{
			return Global::GetDataTypeManager().GetName(pinType.mDataTypeID);
		}
		return pinType.mName;
	}

	eFlowType PinView::GetFlowType() const
	{
		return GetPinType().mFlowType;
	}

	NodeID PinView::GetNodeID() const
	{
		return GetPin().mNodeID;
	}

	PinID PinView::GetID() const
	{
		return myPinID;
	}

	const Pin& PinView::GetPin() const
	{
		return myNodeGraph->mPinManager->myPins.at(myPinID);
	}

	const PinType& PinView::GetPinType() const
	{
		const Pin& pin = GetPin();
		return Global::GetPinTypeManager().GetPinType(pin.mTypeID);
	}
}
