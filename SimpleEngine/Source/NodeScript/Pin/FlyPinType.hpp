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

	struct SetPinValueData final
	{
		NodeGraph* mNodeGraph = nullptr;
		const void* mReadFromDataPtr = nullptr;
		PinID mWriteToPinID = InvalidID<PinID>();
#ifdef FLY_DEBUG
		DataTypeID mReadFromDataTypeID = InvalidID<DataTypeID>();
#endif
	};

	struct SetPinValueFromPinData
	{
		NodeGraph* mNodeGraph = nullptr;
		PinID mWriteToPinID = InvalidID<PinID>();
		PinID mReadFromPinID = InvalidID<PinID>();
	};

	struct PinType final
	{
		std::string mName;
		const eFlowType mFlowType = eFlowType::Input;
		const DataTypeID mDataTypeID = InvalidID<DataTypeID>();
		const SetPinValueInterface mSetPinValueFunction = nullptr;
		const SetPinValueFromPinInterface mSetPinValueFromPinFunction = nullptr;
		MemoryPoolID mDefaultValueID = InvalidID<MemoryPoolID>();
	};


	[[nodiscard]] constexpr eFlowType InvertFlowType(const eFlowType aFlowType)
	{
		return aFlowType == eFlowType::Input ? eFlowType::Output : eFlowType::Input;
	}

	[[nodiscard]] std::string PinFlowTypeToString(const eFlowType aPinType);

	[[nodiscard]] eFlowType StringToPinFlowType(const std::string& aName);

	template<typename T>
	[[nodiscard]] constexpr decltype(auto) SelectByFlowType(eFlowType aFlowType, T&& aInputValue, T&& aOutputValue)
	{
		return aFlowType == eFlowType::Input ? std::forward<T>(aInputValue) : std::forward<T>(aOutputValue);
	}

}