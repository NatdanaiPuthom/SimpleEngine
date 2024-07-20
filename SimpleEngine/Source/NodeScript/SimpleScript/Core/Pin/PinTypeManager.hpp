#pragma once
#include "../ScriptDefines.hpp"
#include "PinType.hpp"
#include <string>
#include <vector>

namespace SCR
{
	class PinTypeManager
	{
	public:

		PinTypeManager();
		~PinTypeManager();

		PinTypeID Create(std::string_view aName, eFlowType aFlowType, DataTypeID aDataTypeID, PinSetFunction aSetFunction);

		template<typename T>
		PinTypeID Create(std::string_view aName, eFlowType aFlowType, PinSetFunction aSetFunction)
		{
			return Create(aName, aFlowType, typeid(T).hash_code(), aSetFunction);
		}

		PinType& GetPinType(PinTypeID anID);

	private:
		std::vector<PinType> mPinTypes;
	};
}