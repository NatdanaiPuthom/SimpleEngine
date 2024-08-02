#include "FlyPinView.hpp"
#include "../Graph/FlyNodeGraph.hpp"
#include "../Global/FlyGlobal.hpp"
#include "../Pin/FlyPinTypeManager.hpp"
#include "../DataType/FlyDataTypeManager.hpp"

namespace FLY_NAMESPACE
{

	PinView::PinView(const PinID aPinID, const NodeGraph& aNodeGraph)
		: mPinID(aPinID)
		, mNodeGraph(&aNodeGraph)
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
		return mPinID;
	}

	PinView::operator bool() const
	{
		return mPinID != InvalidID<PinID>();
	}

	const Pin& PinView::GetPin() const
	{
		return mNodeGraph->mPins.at(mPinID);
	}

	const PinType& PinView::GetPinType() const
	{
		const Pin& pin = GetPin();
		return Global::GetPinTypeManager().GetPinType(pin.mTypeID);
	}
}
