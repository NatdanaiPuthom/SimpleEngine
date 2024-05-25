#pragma once
#include "../ScriptDefines.h"
#include <string>

namespace SCR
{
	enum class ePinFlowType
	{
		NotSet,
		Input,
		Output,
	};

	using PinSetFunction = FuncPtr<void, const PinID, const InternalExecutionContext&, const void*>;

	struct PinType
	{
		std::string name;
		const ePinFlowType flowType = ePinFlowType::NotSet;
		const DataTypeID dataTypeID = InvalidID<DataTypeID>();
		const PinSetFunction setFunction = nullptr;
	};

	constexpr ePinFlowType InvertPinType(const ePinFlowType aPinType)
	{
		return aPinType == ePinFlowType::Input ? ePinFlowType::Output : ePinFlowType::Input;
	}

	std::string PinFlowTypeToString(const ePinFlowType aPinType);

	ePinFlowType StringToPinFlowType(const std::string& aName);

	std::string GetPinLabel(const PinType& aPinType);

}