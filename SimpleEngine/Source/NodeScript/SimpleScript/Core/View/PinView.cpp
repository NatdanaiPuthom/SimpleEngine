#include "PinView.h"
#include "../ScriptNodeGraph.h"
#include "../Global/ScriptGlobal.h"
#include "../Pin/PinTypeManager.h"
#include "../DataType/DataTypeManager.h"

namespace SCR
{

	PinView::PinView(const PinID aPinID, const NodeGraph& aNodeGraph)
		: myPinID(aPinID)
		, myNodeGraph(&aNodeGraph)
	{
	}
	const std::vector<PinID>& PinView::GetConnectedPinIDs() const
	{
		return GetPin().connectedPinIDs;
	}

	DataTypeID PinView::GetDataTypeID() const
	{
		return GetPinType().dataTypeID;
	}

	const std::string& PinView::GetPinTypeName() const
	{
		const PinType& pinType = GetPinType();
		if (pinType.name == "#T")
		{
			return Global::GetDataTypeManager().GetName(pinType.dataTypeID);
		}
		return pinType.name;
	}

	eFlowType PinView::GetFlowType() const
	{
		return GetPinType().flowType;
	}

	NodeID PinView::GetNodeID() const
	{
		return GetPin().nodeID;
	}

	PinID PinView::GetID() const
	{
		return myPinID;
	}

	const Pin& PinView::GetPin() const
	{
		return myNodeGraph->myPinManager->myPins.at(myPinID);
	}

	const PinType& PinView::GetPinType() const
	{
		const Pin& pin = GetPin();
		return Global::GetPinTypeManager().GetPinType(pin.typeID);
	}
}
