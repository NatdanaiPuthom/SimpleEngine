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
		const TypeContainer<PinID, Pin, PinType> tC(myPins, [](const Pin& aPin) -> const PinType& { return PinTypeManager::GetPinType(aPin.typeID); });
			return tC.begin();
	}

	TypeContainer<PinID, Pin, PinType>::ConstIterator PinManager::end() const
	{
		const TypeContainer<PinID, Pin, PinType> tC(myPins, [](const Pin& aPin) -> const PinType& { return PinTypeManager::GetPinType(aPin.typeID); });
		return tC.end();
	}
}
