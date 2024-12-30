#pragma once
#include "../FlyDefines.hpp"
#include "../Graph/FlyNodeGraphVariant.hpp"
#include <string>
#include <vector>

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
		NodeGraphVariantHandle mNodeGraphVariantHandle;
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
		const DataTypeID mDataTypeID = InvalidID<DataTypeID>();
		const GenericDataTypeID mGenericDataTypeID;
		const SetPinValueInterface mSetPinValueFunction = nullptr;
		const SetPinValueFromPinInterface mSetPinValueFromPinFunction = nullptr;
		MemoryPoolID mDefaultValueID = InvalidID<MemoryPoolID>();
		std::vector<PinTypeID> mSubPinTypeIDs;
		const eFlowType mFlowType = eFlowType::Input;
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