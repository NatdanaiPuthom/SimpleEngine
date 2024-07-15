#pragma once
#include "../ScriptDefines.h"
#include <string>

namespace SCR
{
	enum class eFlowType
	{
		Input,
		Output,
	};


	struct PinSetData
	{
		PinID id;
		const void* value;
#ifdef _DEBUG
		DataTypeID dataTypeID;
#endif
	};

	using PinSetFunction = FuncPtr<void, const PinSetData&, const InternalExecutionContext&>;

	struct PinType
	{
		std::string name;
		const eFlowType flowType = eFlowType::Input;
		const DataTypeID dataTypeID = InvalidID<DataTypeID>();
		const PinSetFunction setFunction = nullptr;
	};


	constexpr eFlowType InvertFlowType(const eFlowType aPinType)
	{
		return aPinType == eFlowType::Input ? eFlowType::Output : eFlowType::Input;
	}

	std::string PinFlowTypeToString(const eFlowType aPinType);

	eFlowType StringToPinFlowType(const std::string& aName);

	std::string GetPinLabel(const PinType& aPinType);

	template<typename T>
	decltype(auto) SelectByFlowType(eFlowType aFlowType, T&& aInputValue, T&& aOutputValue)
	{
		return aFlowType == eFlowType::Input ? aInputValue : aOutputValue;
	}

}