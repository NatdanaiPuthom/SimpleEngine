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

		static PinTypeID Create(const std::string& aName, ePinFlowType aFlowType, DataTypeID aHashID, PinSetFunction aSetFunction);

		template<ePinFlowType FlowType, typename T>
		static PinTypeID Create(const std::string& aName, PinSetFunction aSetFunction)
		{
			return Create(aName, FlowType, typeid(T).hash_code(), aSetFunction);
		}

		static PinType& GetPinType(const PinTypeID anID);

	private:
		std::vector<PinType> myPinTypes;
	};
}