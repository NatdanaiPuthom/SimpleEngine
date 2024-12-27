#include "FlyPinTypeFacade.hpp"
#include "../Pin/FlyPinTypeManager.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{

	PinTypeFacade::PinTypeFacade(const PinTypeID anID)
		: mPinTypeID(anID)
	{
	}

	const std::string& PinTypeFacade::GetName() const
	{
		return GetPinType().mName;
	}

	eFlowType PinTypeFacade::GetFlowType() const
	{
		return GetPinType().mFlowType;
	}

	DataTypeID PinTypeFacade::GetDataTypeID() const
	{
		return GetPinType().mDataTypeID;
	}

	PinTypeID PinTypeFacade::GetID() const
	{
		return mPinTypeID;
	}

	void PinTypeFacade::SetName(const std::string_view aName)
	{
		PinType& pinType = GetPinType();

		pinType.mName = aName;
	}

	PinType& PinTypeFacade::GetPinType() 
	{
		return Internal::GetPinTypeManager().GetPinType(mPinTypeID);
	}

	const PinType& PinTypeFacade::GetPinType() const
	{
		return Internal::GetPinTypeManager().GetPinType(mPinTypeID);
	}
}
