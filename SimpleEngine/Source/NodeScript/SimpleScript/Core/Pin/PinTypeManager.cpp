#include "PinTypeManager.h"

namespace SCR
{
	PinTypeID PinTypeManager::Create(const std::string& aName, ePinFlowType aFlowType, DataTypeID aHashID, PinSetFunction aSetFunction)
	{
		PinTypeID id = myPinTypes.size();
		myPinTypes.emplace_back(aName, aFlowType, aHashID, aSetFunction);
		return id;
	}

	PinType& PinTypeManager::GetPinType(const PinTypeID anID)
	{
		return myPinTypes.at(anID);
	}

	void PinTypeManager::Destroy()
	{
		myPinTypes.clear();
		myPinTypes.~vector();
	}
}