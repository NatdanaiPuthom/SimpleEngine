#include "FlyPinTypeProxy.hpp"
#include "../Pin/FlyPinTypeManager.hpp"
#include "../Internal/FlyInternal.hpp"

namespace FLY_NAMESPACE
{

	PinTypeProxy::PinTypeProxy(const PinTypeID anID)
		: mPinTypeID(anID)
	{
	}

	const std::string& PinTypeProxy::GetName() const
	{
		return GetPinType().mName;
	}

	eFlowType PinTypeProxy::GetFlowType() const
	{
		return GetPinType().mFlowType;
	}

	/*DataTypeID PinTypeProxy::GetDataTypeID() const
	{
		return GetPinType().mDataTypeID;
	}*/

	GenericDataTypeID PinTypeProxy::GetDataTypeID() const
	{
		return GetPinType().mGenericDataTypeID;
	}

	PinTypeID PinTypeProxy::GetID() const
	{
		return mPinTypeID;
	}

	void PinTypeProxy::SetName(const std::string_view aName)
	{
		PinType& pinType = GetPinType();

		pinType.mName = aName;
	}

	PinType& PinTypeProxy::GetPinType() 
	{
		return Internal::GetPinTypeManager().GetPinType(mPinTypeID);
	}

	const PinType& PinTypeProxy::GetPinType() const
	{
		return Internal::GetPinTypeManager().GetPinType(mPinTypeID);
	}
}
