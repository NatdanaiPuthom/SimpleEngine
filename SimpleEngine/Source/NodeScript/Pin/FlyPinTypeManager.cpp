#include "FlyPinTypeManager.hpp"

namespace FLY_NAMESPACE
{

	PinTypeManager::PinTypeManager()
	{
	}

	PinTypeManager::~PinTypeManager()
	{
	}

	PinTypeID PinTypeManager::CreatePinType(std::string_view aName, const eFlowType aFlowType, const DataTypeID aDataTypeID, SetPinValueInterface aSetPinValueFunction, SetPinValueFromPinInterface aSetPinValueFromPinFunction, const MemoryPoolID aDefaultValueMemoryID)
	{
		const PinTypeID id{ mPinTypes.size() };
		mPinTypes.push_back(PinType
			{
				.mName = std::string(aName),
				.mDataTypeID = aDataTypeID,
				.mSetPinValueFunction = aSetPinValueFunction,
				.mSetPinValueFromPinFunction = aSetPinValueFromPinFunction,
				.mDefaultValueID = aDefaultValueMemoryID,
				.mFlowType = aFlowType,
			});
		return id;
	}
}