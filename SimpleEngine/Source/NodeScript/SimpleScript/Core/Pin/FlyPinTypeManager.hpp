#pragma once
#include "../FlyDefines.hpp"
#include "FlyPinType.hpp"
#include <string>
#include <vector>

namespace FLY_NAMESPACE
{
	class PinTypeManager
	{
	public:

		PinTypeManager();
		~PinTypeManager();

		PinTypeID Create(std::string_view aName, eFlowType aFlowType, DataTypeID aDataTypeID, PinSetFunction aSetFunction, MemoryPoolID aDefaultValueMemoryID = InvalidID<MemoryPoolID>());

		template<typename T>
		PinTypeID Create(std::string_view aName, eFlowType aFlowType, PinSetFunction aSetFunction, MemoryPoolID aDefaultValueMemoryID = InvalidID<MemoryPoolID>())
		{
			return Create(aName, aFlowType, GetDataTypeID<T>(), aSetFunction, aDefaultValueMemoryID);
		}

		PinType& GetPinType(PinTypeID anID);

	private:
		std::vector<PinType> mPinTypes;
	};
}