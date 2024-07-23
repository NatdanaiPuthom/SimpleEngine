#include "FlyPinTypeManager.hpp"

namespace FLY_NAMESPACE
{

	PinTypeManager::PinTypeManager()
	{
	}

	PinTypeManager::~PinTypeManager()
	{
	}

	PinTypeID PinTypeManager::Create(std::string_view aName, eFlowType aFlowType, DataTypeID aDataTypeID, PinSetFunction aSetFunction)
	{
		const PinTypeID id = mPinTypes.size();
		mPinTypes.emplace_back(std::string(aName), aFlowType, aDataTypeID, aSetFunction);
		return id;
	}

	PinType& PinTypeManager::GetPinType(const PinTypeID anID)
	{
		return mPinTypes.at(anID);
	}
}