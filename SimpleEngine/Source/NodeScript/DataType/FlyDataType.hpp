#pragma once
#include "../FlyDefines.hpp"
#include "FlyVariable.hpp"
#include "nlohmann/json.hpp"

namespace FLY_NAMESPACE
{
	using AllocateInterface = void(*)(void* aDataPtr, const void* aDefaultValue);
	using ReleaseInterface = void(*)(void* aDataPtr);
	using CopyInterface = void(*)(void* aDestination, const void* aSource);
	using SwapInterface = void(*)(void* aDataPtr1, void* aDataPtr2);
	using EqualsInterface = bool(*)(const void* aDataPtr1, const void* aDataPtr2);

	struct FundamentalInterface final
	{
		const AllocateInterface allocate = nullptr;
		const ReleaseInterface release = nullptr;
		const CopyInterface copy = nullptr;
		const SwapInterface swap = nullptr;
		const EqualsInterface equals = nullptr;
	};

	using ViewAndEditInterface = ViewAndEditResult(*)(void* aDataPtr);
	using ViewInterface = void(*)(const void* aDataPtr);
	using SaveInterface = void(*)(nlohmann::json& aSaveObject, const void* aDataPtr);
	using LoadInterface = void(*)(const nlohmann::json& aLoadObject, void* aDataPtr);

	struct FunctionInterface final
	{
		const ViewAndEditInterface viewAndEdit = nullptr;
		const ViewInterface view = nullptr;
		const SaveInterface save = nullptr;
		const LoadInterface load = nullptr;
	};

	struct ExecutionInterface final
	{
		const SetPinValueInterface setInputPinValue = nullptr;
		const SetPinValueInterface setOutputPinValue = nullptr;
		const SetPinValueFromPinInterface setInputPinValueFromPin = nullptr;
		const SetPinValueFromPinInterface setOutputPinValueFromPin = nullptr;
	};

	struct DataTypeInterface final
	{
		const FundamentalInterface fundamental;
		const FunctionInterface function;
		const ExecutionInterface execution;
	};

	enum class eDataTypeTrait
	{
		None = 0,
		Fundamental = 1 << 0,
		ViewAndEditable = 1 << 1,
		SaveLoadable = 1 << 2,
		Targetable = 1 << 3,
		Pointer = 1 << 4,
		TriviallyCopyable = 1 << 5,
		All = Fundamental | ViewAndEditable | SaveLoadable | Targetable
	};

	struct DataType
	{
		const std::string mName;
		const size_t mSize = 0;
		Color mColor;
		const std::type_info* mTypeInfo;
		const DataTypeInterface mInterface;
		std::vector<Variable> mVariables;
		std::vector<NodeTypeID> mNodeTypeIDs;
		const DataTypeID mToPointerDataTypeID = InvalidID<DataTypeID>();
		const DataTypeID mToValueDataTypeID = InvalidID<DataTypeID>();
		const eDataTypeTrait mTypeTraits = eDataTypeTrait::None;
	};

	struct TemplateDataType
	{
		const std::string mName;
	};
}