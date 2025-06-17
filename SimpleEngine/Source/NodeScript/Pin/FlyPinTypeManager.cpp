#include "FlyPinTypeManager.hpp"

namespace FLY_NAMESPACE
{

	PinTypeManager::PinTypeManager()
	{
	}

	PinTypeManager::~PinTypeManager()
	{
	}

	//PinTypeID PinTypeManager::CreatePinType(std::string_view aName, const eFlowType aFlowType, const DataTypeID aDataTypeID, SetPinValueInterface aSetPinValueFunction, SetPinValueFromPinInterface aSetPinValueFromPinFunction, const MemoryPoolID aDefaultValueMemoryID)
	//{
	//	const PinTypeID id{ mPinTypes.size() };
	//	mPinTypes.push_back(PinType
	//		{
	//			.mName = std::string(aName),
	//			//.mDataTypeID = aDataTypeID,
	//			.mGenericDataTypeID = aDataTypeID,
	//			.mSetPinValueFunction = aSetPinValueFunction,
	//			.mSetPinValueFromPinFunction = aSetPinValueFromPinFunction,
	//			.mDefaultValueID = aDefaultValueMemoryID,
	//			.mFlowType = aFlowType,
	//		});
	//	return id;
	//}

	PinTypeID PinTypeManager::CreatePinType(std::string_view aName, const eIODirection aIODirection, const GenericDataTypeID aDataTypeID, SetPinValueF aSetPinValueFunction, SetPinValueFromPinF aSetPinValueFromPinFunction, const MemoryPoolID aDefaultValueMemoryID)
	{
		const PinTypeID id{ mPinTypes.size() };
		mPinTypes.push_back(PinType(std::string(aName), aDataTypeID, aSetPinValueFunction, aSetPinValueFromPinFunction, aDefaultValueMemoryID, aIODirection));
		return id;
	}
}