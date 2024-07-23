#pragma once
#include "../FlyDefines.hpp"

namespace FLY_NAMESPACE
{
	using EditInterface = bool(*)(void* aDataPtr);
	using SaveInterface = void(*)(nlohmann::json& aSaveObject, const void* aDataPtr);
	using LoadInterface = void(*)(const nlohmann::json& aLoadObject, void* aDataPtr);
	using AllocateInterface = void (*)(void* aDataPtr, const void* aDefaultValue);
	using ReleaseInterface = void (*)(void* aDataPtr);
	using CopyInterface = void(*)(void* aDestination, const void* aSource);
	using SwapInterface = void(*)(void* aDataPtr1, void* aDataPtr2);

	struct FunctionInterface
	{
		const EditInterface edit;
		const SaveInterface save;
		const LoadInterface load;

		operator bool() const
		{
			return edit && save && load;
		}
	};


	struct CreationInterface
	{
		const AllocateInterface allocate;
		const ReleaseInterface release;
		const CopyInterface copy;
		const SwapInterface swap;

		operator bool() const
		{
			return allocate && copy && swap;
		}
	};

	struct DataTypeInterface
	{
		const FunctionInterface function;
		const CreationInterface creation;
		operator bool() const
		{
			return function && creation;
		}
	};

	struct Property
	{
		const std::string mName;
		const DataTypeID mTypeID;
		const size_t mByteOffset;
	};

	enum class eDataTypeTrait
	{
		None = 0,
		Fundamental = 1 << 0,
		Editable = 1 << 1,
		SaveLoadable = 1 << 2,
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
		std::vector<Property> mProperties;
		std::vector<NodeTypeID> mNodeTypeIDs;
	};

	struct TemplateDataType
	{
		const std::string mName;
	};
}