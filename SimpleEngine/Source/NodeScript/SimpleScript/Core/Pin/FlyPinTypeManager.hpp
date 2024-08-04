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

		PinTypeID Create(std::string_view aName, eFlowType aFlowType, DataTypeID aDataTypeID, SetPinDataInterface aSetFunction, MemoryPoolID aDefaultValueMemoryID = InvalidID<MemoryPoolID>());

		template<typename T>
		PinTypeID Create(std::string_view aName, eFlowType aFlowType, SetPinDataInterface aSetFunction, MemoryPoolID aDefaultValueMemoryID = InvalidID<MemoryPoolID>())
		{
			return Create(aName, aFlowType, GetDataTypeID<T>(), aSetFunction, aDefaultValueMemoryID);
		}

		PinType& GetPinType(PinTypeID anID);

	private:
		std::vector<PinType> mPinTypes;
	};
}