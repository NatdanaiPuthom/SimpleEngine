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

		//PinTypeID CreatePinType(std::string_view aName, eFlowType aFlowType, DataTypeID aDataTypeID, SetPinValueFunction aSetPinValueFunction, SetPinValueFromPinInterface aSetPinValueFromPinInterface, MemoryPoolID aDefaultValueMemoryID = InvalidID<MemoryPoolID>());

		template<typename T>
		[[nodiscard]] PinTypeID CreatePinType(std::string_view aName, eIODirection aIODirection, SetPinValueF aSetPinValueFunction, SetPinValueFromPinF aSetPinValueFromPinFunction, MemoryPoolID aDefaultValueMemoryID = InvalidID<MemoryPoolID>())
		{
			return CreatePinType(aName, aIODirection, GenericDataTypeID{ GetDataTypeID<T>() }, aSetPinValueFunction, aSetPinValueFromPinFunction, aDefaultValueMemoryID);
		}

		[[nodiscard]] PinTypeID CreatePinType(std::string_view aName, eIODirection aIODirection, GenericDataTypeID aDataTypeID, SetPinValueF aSetPinValueFunction, SetPinValueFromPinF aSetPinValueFromPinFunction, MemoryPoolID aDefaultValueMemoryID = InvalidID<MemoryPoolID>());

		[[nodiscard]] PinType& GetPinType(PinTypeID aID)
		{
			return mPinTypes[aID];
		}

		[[nodiscard]] const PinType& GetPinType(PinTypeID aID) const
		{
			return mPinTypes[aID];
		}

	private:
		std::vector<PinType> mPinTypes;
	};
}