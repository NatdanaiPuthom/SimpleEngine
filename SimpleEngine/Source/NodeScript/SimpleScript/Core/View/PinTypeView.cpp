#include "PinTypeView.h"
#include "../Global/ScriptGlobal.h"
#include "../Pin/PinTypeManager.h"

namespace SCR
{

	PinTypeView::PinTypeView(PinTypeID anID)
		: myPinTypeID(anID)
	{
	}

	const std::string& PinTypeView::GetName() const
	{
		return GetPinType().name;
	}

	eFlowType PinTypeView::GetFlowType() const
	{
		return GetPinType().flowType;
	}

	DataTypeID PinTypeView::GetDataTypeID() const
	{
		return GetPinType().dataTypeID;
	}

	PinTypeID PinTypeView::GetID() const
	{
		return myPinTypeID;
	}

	const PinType& PinTypeView::GetPinType() const
	{
		return Global::GetPinTypeManager().GetPinType(myPinTypeID);
	}
}
