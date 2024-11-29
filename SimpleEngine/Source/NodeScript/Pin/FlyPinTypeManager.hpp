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

		PinTypeID Create(std::string_view aName, eFlowType aFlowType, DataTypeID aDataTypeID, SetPinValueInterface aSetPinValueFunction, SetPinValueFromPinInterface aSetPinValueFromPinInterface, MemoryPoolID aDefaultValueMemoryID = InvalidID<MemoryPoolID>());

		template<typename T>
		PinTypeID Create(std::string_view aName, eFlowType aFlowType, SetPinValueInterface aSetPinValueFunction, SetPinValueFromPinInterface aSetPinValueFromPinInterface, MemoryPoolID aDefaultValueMemoryID = InvalidID<MemoryPoolID>())
		{
			return Create(aName, aFlowType, GetDataTypeID<T>(), aSetPinValueFunction, aSetPinValueFromPinInterface, aDefaultValueMemoryID);
		}

		PinType& GetPinType(PinTypeID anID);

	private:
		std::vector<PinType> mPinTypes;
	};
}