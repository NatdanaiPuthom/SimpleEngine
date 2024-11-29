#include "FlyPinTypeManager.hpp"

namespace FLY_NAMESPACE
{

	PinTypeManager::PinTypeManager()
	{
	}

	PinTypeManager::~PinTypeManager()
	{
	}

	PinTypeID PinTypeManager::Create(std::string_view aName, const eFlowType aFlowType, const DataTypeID aDataTypeID, SetPinValueInterface aSetPinValueFunction, SetPinValueFromPinInterface aSetPinValueFromPinFunction, const MemoryPoolID aDefaultValueMemoryID)
	{
		const PinTypeID id = mPinTypes.size();
		mPinTypes.emplace_back(PinType
			{
				.mName = std::string(aName),
				.mFlowType = aFlowType,
				.mDataTypeID = aDataTypeID,
				.mSetPinValueFunction = aSetPinValueFunction,
				.mSetPinValueFromPinFunction = aSetPinValueFromPinFunction,
				.mDefaultValueID = aDefaultValueMemoryID
			});
		return id;
	}

	PinType& PinTypeManager::GetPinType(const PinTypeID anID)
	{
		return mPinTypes.at(anID);
	}
}