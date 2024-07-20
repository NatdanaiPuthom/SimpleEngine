#include "PinTypeView.hpp"
#include "../Global/ScriptGlobal.hpp"
#include "../Pin/PinTypeManager.hpp"

namespace FLY_NAMESPACE
{

	PinTypeView::PinTypeView(PinTypeID anID)
		: mPinTypeID(anID)
	{
	}

	const std::string& PinTypeView::GetName() const
	{
		return GetPinType().mName;
	}

	eFlowType PinTypeView::GetFlowType() const
	{
		return GetPinType().mFlowType;
	}

	DataTypeID PinTypeView::GetDataTypeID() const
	{
		return GetPinType().mDataTypeID;
	}

	PinTypeID PinTypeView::GetID() const
	{
		return mPinTypeID;
	}

	const PinType& PinTypeView::GetPinType() const
	{
		return Global::GetPinTypeManager().GetPinType(mPinTypeID);
	}
}
