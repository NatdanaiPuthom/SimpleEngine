#pragma once
#include "../FlyDefines.hpp"
#include <string>

namespace FLY_NAMESPACE
{
	enum class eFlowType
	{
		Input,
		Output,
	};

	class NodeGraph;

	struct PinSetData
	{
		PinID mID;
		NodeGraph* mNodeGraph;
		const void* mValue;
#ifdef _DEBUG
		DataTypeID mDataTypeID;
#endif
	};

	using PinSetFunction = FuncPtr<void, const PinSetData&, const InternalExecutionContext&>;

	struct PinType
	{
		std::string mName;
		const eFlowType mFlowType = eFlowType::Input;
		const DataTypeID mDataTypeID = InvalidID<DataTypeID>();
		const PinSetFunction mSetFunction = nullptr;
		MemoryPoolID mDefaultValueID = InvalidID<MemoryPoolID>();
	};


	constexpr eFlowType InvertFlowType(const eFlowType aFlowType)
	{
		return aFlowType == eFlowType::Input ? eFlowType::Output : eFlowType::Input;
	}

	std::string PinFlowTypeToString(const eFlowType aPinType);

	eFlowType StringToPinFlowType(const std::string& aName);

	template<typename T>
	[[nodiscard]] constexpr decltype(auto) SelectByFlowType(eFlowType aFlowType, T&& aInputValue, T&& aOutputValue)
	{
		return aFlowType == eFlowType::Input ? std::forward<T>(aInputValue) : std::forward<T>(aOutputValue);
	}

}