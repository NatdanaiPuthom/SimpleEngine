#pragma once
#include "../ScriptDefines.h"
#include "../Utilities/MetaScript.h"
#include "PinType.h"
#include <string>
#include <vector>

namespace SCR
{
	class PinTypeManager
	{
	public:

		PinTypeManager();
		~PinTypeManager();

		static PinTypeManager& GetInstance();

		static PinTypeID Create(const std::string& aName, eFlowType aFlowType, DataTypeID aDataTypeID, PinSetFunction aSetFunction);

		template<eFlowType FlowType, typename T>
		static PinTypeID Create(const std::string& aName, PinSetFunction aSetFunction)
		{
			return Create(aName, FlowType, typeid(T).hash_code(), aSetFunction);
		}

		static PinType& GetPinType(PinTypeID anID);

	private:
		std::vector<PinType> myPinTypes;
	};
}