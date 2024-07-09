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

	TypeContainer<PinID, const Pin, const PinType>::Iterator PinManager::begin() const
	{
		auto f = [](const Pin& aPin) -> const PinType& { return PinTypeManager::GetPinType(aPin.typeID); };
		const TypeContainer<PinID, const Pin, const PinType> tC(myPins, f);
		return tC.begin();
	}

	TypeContainer<PinID, const Pin, const PinType>::Iterator PinManager::end() const
	{
		auto f = [](const Pin& aPin) -> const PinType& { return PinTypeManager::GetPinType(aPin.typeID); };
		const TypeContainer<PinID, const Pin, const PinType> tC(myPins, f);
		return tC.end();
	}
}
