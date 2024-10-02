#pragma once
#include "../FlyDefines.hpp"
#include "FlyVariable.hpp"
#include "nlohmann/json.hpp"

namespace nlohamnn
{
	class json;
}

namespace FLY_NAMESPACE
{
	using EditInterface = bool(*)(void* aDataPtr);
	using SaveInterface = void(*)(nlohmann::json& aSaveObject, const void* aDataPtr);
	using LoadInterface = void(*)(const nlohmann::json& aLoadObject, void* aDataPtr);
	using AllocateInterface = void(*)(void* aDataPtr, const void* aDefaultValue);
	using ReleaseInterface = void(*)(void* aDataPtr);
	using CopyInterface = void(*)(void* aDestination, const void* aSource);
	using SwapInterface = void(*)(void* aDataPtr1, void* aDataPtr2);

	struct FunctionInterface final
	{
		const EditInterface edit;
		const SaveInterface save;
		const LoadInterface load;
	};


	struct CreationInterface final
	{
		const AllocateInterface allocate;
		const ReleaseInterface release;
		const CopyInterface copy;
		const SwapInterface swap;
	};

	struct ExecutionInterface final
	{
		const SetPinDataInterface setInputPinData;
		const SetPinDataInterface setOutputPinData;
	};

	struct DataTypeInterface final
	{
		const FunctionInterface function;
		const CreationInterface creation;
		const ExecutionInterface execution;
	};

	enum class eDataTypeTrait
	{
		None = 0,
		Fundamental = 1 << 0,
		Editable = 1 << 1,
		SaveLoadable = 1 << 2,
		Pointer = 1 << 4,
		Targetable = 1 << 3,
		All = Fundamental | Editable | SaveLoadable | Targetable
	};

	struct DataType
	{
		const std::string mName;
		const size_t mSize;
		const Color mColor;
		const std::type_info* mTypeInfo;
		const eDataTypeTrait mTypeTraits;
		const DataTypeInterface mInterface;
		std::vector<Variable> mVariables;
		std::vector<NodeTypeID> mNodeTypeIDs;
	};

	struct TemplateDataType
	{
		const std::string mName;
	};
}