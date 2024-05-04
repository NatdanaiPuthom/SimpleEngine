#include "PinManager.h"
#include "PinTypeManager.h"

namespace SCR
{

	PinManager::PinManager()
	{
	}

	PinManager::~PinManager()
	{
	}

	TypeContainer<PinID, Pin, PinType>::ConstIterator PinManager::begin() const
	{
		return TypeContainer<PinID, Pin, PinType>(myPins, [](const Pin& aPin) -> const PinType& { return PinTypeManager::GetPinType(aPin.typeID); }).begin();
	}

	TypeContainer<PinID, Pin, PinType>::ConstIterator PinManager::end() const
	{
		return TypeContainer<PinID, Pin, PinType>(myPins, [](const Pin& aPin) -> const PinType& { return PinTypeManager::GetPinType(aPin.typeID); }).end();
	}
}
