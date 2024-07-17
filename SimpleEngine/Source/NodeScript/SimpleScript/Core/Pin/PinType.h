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


	constexpr eFlowType InvertFlowType(const eFlowType aFlowType)
	{
		return aFlowType == eFlowType::Input ? eFlowType::Output : eFlowType::Input;
	}

	std::string PinFlowTypeToString(const eFlowType aPinType);

	eFlowType StringToPinFlowType(const std::string& aName);

	template<typename T>
	decltype(auto) SelectByFlowType(eFlowType aFlowType, T&& aInputValue, T&& aOutputValue)
	{
		return aFlowType == eFlowType::Input ? std::forward<T>(aInputValue) : std::forward<T>(aOutputValue);
	}

}