#include "PinTypeManager.h"
#include "ScriptFoundation.h"
#include "Type/ScriptTypeManager.h"

namespace SCR
{

	PinTypeManager::PinTypeManager()
	{
	}

	PinTypeManager::~PinTypeManager()
	{
	}

	PinTypeManager& PinTypeManager::GetInstance()
	{
		return ScriptFoundation::GetInstance().GetTypeManager().GetPinTypeManager();
	}

	PinTypeID PinTypeManager::Create(const std::string& aName, ePinFlowType aFlowType, DataTypeID aHashID, PinSetFunction aSetFunction)
	{
		PinTypeManager& pinTypeManager = GetInstance();
		PinTypeID id = pinTypeManager.myPinTypes.size();
		pinTypeManager.myPinTypes.emplace_back(aName, aFlowType, aHashID, aSetFunction);
		return id;
	}

	PinType& PinTypeManager::GetPinType(const PinTypeID anID)
	{
		PinTypeManager& pinTypeManager = GetInstance();
		return pinTypeManager.myPinTypes.at(anID);
	}
}