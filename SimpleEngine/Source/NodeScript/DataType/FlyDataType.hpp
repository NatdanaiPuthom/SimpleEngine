#pragma once
#include "../FlyDefines.hpp"
#include "nlohmann/json.hpp"
#include "FlyDataTypeTrait.hpp"
#include "FlyVariableContainer.hpp"

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

	using EditorTextFunction = void(*)(const std::string& aText);
	using ViewAndEditInterface = ViewAndEditResult(*)(void* aDataPtr, EditorTextFunction aNullptrFunction);
	using ViewInterface = void(*)(const void* aDataPtr, EditorTextFunction aNullptrFunction);
	using SaveInterface = void(*)(const void* aDataPtr, nlohmann::json& aSaveObject);
	using LoadInterface = void(*)(void* aDataPtr, const nlohmann::json& aLoadObject);

	struct FunctionInterface final
	{
		ViewAndEditInterface viewAndEdit = nullptr;
		ViewInterface view = nullptr;
		SaveInterface save = nullptr;
		LoadInterface load = nullptr;
	};

	struct ExecutionInterface final
	{
		SetPinValueInterface setInputPinValue = nullptr;
		SetPinValueInterface setOutputPinValue = nullptr;
		SetPinValueFromPinInterface setInputPinValueFromPin = nullptr;
		SetPinValueFromPinInterface setOutputPinValueFromPin = nullptr;
	};

	struct DataTypeInterface final
	{
		FundamentalInterface fundamental;
		FunctionInterface function;
		ExecutionInterface execution;
	};

	struct DataType final
	{
		std::string mName;
		size_t mSize = 0;
		size_t mAlignment = 0;
		Color mColor;
		const std::type_info* mTypeInfo = nullptr;
		DataTypeInterface mInterface;
		VariableContainer mVariableContainer;
		std::vector<NodeTypeID> mNodeTypeIDs;
		DataTypeID mToPointerDataTypeID;
		DataTypeID mToValueDataTypeID;
		eDataTypeTrait mTypeTraits = eDataTypeTrait::None;
	};

	struct TemplateDataType
	{
		const std::string mName;
	};

	enum class eDataTypeRelation : uint8_t
	{
		None,
		Same,
		Pointer_Value,
		Value_Pointer
	};
}