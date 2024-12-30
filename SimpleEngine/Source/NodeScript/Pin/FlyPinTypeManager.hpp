#pragma once
#include "../FlyDefines.hpp"
#include "FlyPinType.hpp"
#include <string>
#include <vector>

namespace FLY_NAMESPACE
{
	class PinTypeManager final
	{
	public:

		PinTypeManager();
		~PinTypeManager();

		PinTypeID CreatePinType(std::string_view aName, eFlowType aFlowType, DataTypeID aDataTypeID, SetPinValueInterface aSetPinValueFunction, SetPinValueFromPinInterface aSetPinValueFromPinInterface, MemoryPoolID aDefaultValueMemoryID = InvalidID<MemoryPoolID>());

		template<typename T>
		PinTypeID CreatePinType(std::string_view aName, eFlowType aFlowType, SetPinValueInterface aSetPinValueFunction, SetPinValueFromPinInterface aSetPinValueFromPinInterface, MemoryPoolID aDefaultValueMemoryID = InvalidID<MemoryPoolID>())
		{
			return CreatePinType(aName, aFlowType, GetDataTypeID<T>(), aSetPinValueFunction, aSetPinValueFromPinInterface, aDefaultValueMemoryID);
		}

		PinTypeID CreatePinType(std::string_view aName, eFlowType aFlowType, GenericDataTypeID aDataTypeID);

		PinType& GetPinType(PinTypeID aID)
		{
			return mPinTypes[aID];
		}
		const PinType& GetPinType(PinTypeID aID) const
		{
			return mPinTypes[aID];
		}

	private:
		std::vector<PinType> mPinTypes;
	};
}