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
		return GetPinType().GetName();
	}

	eIODirection PinTypeProxy::GetIODirection() const
	{
		return GetPinType().GetIODirection();
	}

	/*DataTypeID PinTypeProxy::GetDataTypeID() const
	{
		return GetPinType().mDataTypeID;
	}*/

	GenericDataTypeID PinTypeProxy::GetDataTypeID() const
	{
		return GetPinType().GetDataTypeID();
	}

	PinTypeID PinTypeProxy::GetID() const
	{
		return mPinTypeID;
	}

	void PinTypeProxy::SetName(const std::string_view aName)
	{
		PinType& pinType = GetPinType();

		pinType.SetName(std::string(aName));
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
