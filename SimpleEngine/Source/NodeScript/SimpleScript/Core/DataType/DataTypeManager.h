#pragma once
#include "../ScriptDefines.h"
#include "../Utilities/MetaScript.h"
#include "../Utilities/ScriptUtilities.h"
#include "../Memory/ScriptMemoryPool.h"
#include <nlohmann/json.hpp>

namespace SCR
{
	template<typename T>
	concept Fundamental = std::is_fundamental_v<T>;

	using EditInterface = bool(*)(void*);
	using SaveInterface = void(*)(nlohmann::json&, const void*);
	using LoadInterface = void(*)(const nlohmann::json&, void*);
	using AllocateInterface = MemoryPoolID(*)(MemoryPool&);
	using CopyInterface = void(*)(void*, const void*);
	using SwapInterface = void(*)(void*, void*);

	template<Editable T>
	EditInterface CreateEditInterface()
	{
		return [](void* aDataPtr) -> bool
			{
				T& value = *reinterpret_cast<T*>(aDataPtr);
				return Edit(value);
			};
	}

	template<Savable<nlohmann::json> T>
	SaveInterface CreateSaveInterface()
	{
		return [](nlohmann::json& aJson, const void* aDataPtr) -> void
			{
				const T& value = *reinterpret_cast<const T*>(aDataPtr);
				Save(aJson, value);
			};
	}

	template<Loadable<nlohmann::json> T>
	LoadInterface CreateLoadInterface()
	{
		return [](const nlohmann::json& aJson, void* aDataPtr) -> void
			{
				T& value = *reinterpret_cast<T*>(aDataPtr);
				Load(aJson, value);
			};
	}

	template<typename T>
	EditInterface CreateEditTemplateInterface()
	{
		return [](void* aDataPtr) -> bool
			{
				T& value = *reinterpret_cast<T*>(aDataPtr);
				return EditTemplate(value);
			};
	}

	template<typename T>
	SaveInterface CreateSaveTemplateInterface()
	{
		return [](nlohmann::json& aJson, const void* aDataPtr) -> void
			{
				const T& value = *reinterpret_cast<const T*>(aDataPtr);
				SaveTemplate(aJson, value);
			};
	}

	template<typename T>
	LoadInterface CreateLoadTemplateInterface()
	{
		return [](const nlohmann::json& aJson, void* aDataPtr) -> void
			{
				T& value = *reinterpret_cast<T*>(aDataPtr);
				LoadTemplate(aJson, value);
			};
	}

	template<Fundamental T>
	EditInterface CreateEditInterface()
	{
		return [](void* aDataPtr) -> bool
			{
				T& value = *reinterpret_cast<T*>(aDataPtr);
				return ::Edit(value);
			};
	}

	template<Fundamental T>
	SaveInterface CreateSaveInterface()
	{
		return [](nlohmann::json& aJson, const void* aDataPtr) -> void
			{
				const T& value = *reinterpret_cast<const T*>(aDataPtr);
				::Save(aJson, value);
			};
	}

	template<Fundamental T>
	LoadInterface CreateLoadInterface()
	{
		return [](const nlohmann::json& aJson, void* aDataPtr) -> void
			{
				T& value = *reinterpret_cast<T*>(aDataPtr);
				::Load(aJson, value);
			};
	}


	template<DefaultConstructible T>
	AllocateInterface CreateAllocateInterface()
	{
		return [](MemoryPool& aMemoryPool) -> MemoryPoolID
			{
				return aMemoryPool.Allocate<T>();
			};
	}

	template<Copyable T>
	CopyInterface CreateCopyInterface()
	{
		return [](void* aDestination, const void* aSource)
			{
				T& destination = *reinterpret_cast<T*>(aDestination);
				const T& source = *reinterpret_cast<const T*>(aSource);
				destination = source;
			};
	}

	template<Copyable T>
	SwapInterface CreateSwapInterface()
	{
		return [](void* aDataPtr1, void* aDataPtr2)
			{
				T& value1 = *reinterpret_cast<T*>(aDataPtr1);
				T& value2 = *reinterpret_cast<T*>(aDataPtr2);

				std::swap(value1, value2);
			};
	}

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

	template<typename T>
	FunctionInterface CreateFunctionInterface()
	{
		return FunctionInterface
		{
			CreateEditInterface<T>(),
			CreateSaveInterface<T>(),
			CreateLoadInterface<T>()
		};
	}

	template<typename T>
	CreationInterface CreateCreationInterface()
	{
		return CreationInterface
		{
			CreateAllocateInterface<T>(),
			CreateCopyInterface<T>(),
			CreateSwapInterface<T>()
		};
	}

	template<typename T, bool HasFunctions>
	DataTypeInterface CreateDataTypeInterface()
	{
		if constexpr (HasFunctions)
		{
			return DataTypeInterface
			{
				CreateFunctionInterface<T>(),
				CreateCreationInterface<T>()
			};
		}
		else
		{
			return DataTypeInterface
			{
				FunctionInterface{},
				CreateCreationInterface<T>()
			};
		}
	}

	struct DataType
	{
		const std::string name;
		const ScriptColor color;
		const std::type_info* typeInfo;
		const DataTypeInterface typeInterface;
	};

	struct TemplateDataType
	{
		const std::string name;
	};


	const ScriptColor DefaultColor = ScriptColor(1.f, 1.f, 0.3f);

	class DataTypeManager
	{
		friend class DataTypeRegistry;
	public:

		static bool EditData(DataTypeID aDataTypeID, void* aDataPtr);
		static void SaveData(DataTypeID aDataTypeID, nlohmann::json& aJson, const void* aDataPtr);
		static void LoadData(DataTypeID aDataTypeID, const nlohmann::json& aJson, void* aDataPtr);
		static MemoryPoolID AllocateData(DataTypeID aDataTypeID, MemoryPool& aMemoryPool);
		static void CopyData(DataTypeID aDataTypeID, void* aDestination, const void* aSource);
		static void SwapData(DataTypeID aDataTypeID, void* aDataPtr1, void* aDataPtr2);
		static const std::string& GetName(DataTypeID aDataTypeID);

		static DataTypeID GetDataTypeIDByName(const std::string& aName);

		static const std::unordered_map<DataTypeID, DataType>& GetObjectTypes();
		static std::unordered_map<DataTypeID, const DataType*> GetFunctionObjectTypes();

		static ScriptColor GetColor(const DataTypeID aDataTypeID);
		static ScriptColor GetSelectionColor(const DataTypeID aDataTypeID);
		static ScriptColor GetHoverColor(const DataTypeID aDataTypeID);

		static void Destroy();

	private:

		template<Scriptable<nlohmann::json> T>
		static void Register(const std::string& aName, const ScriptColor& aColor = DefaultColor);

		template<Fundamental T>
		static void Register(const std::string& aName, const ScriptColor& aColor = DefaultColor);

		template<typename T>
		static void RegisterNonSerializableType(const std::string& aName, const ScriptColor& aColor);

		template<template<typename> typename TemplateType>
		static void RegisterTemplateType(const std::string& aName);

		template<typename T>
		static void RegisterTemplateSpecification(const std::string& aName, const ScriptColor& aColor = DefaultColor);

		template<CleanType T>
		static void RegisterInternal(const std::string& aName, const ScriptColor& aColor, const DataTypeInterface& anInterface);

	private:

		inline static std::unordered_map<DataTypeID, DataType> myDataTypes;
		inline static std::unordered_map<DataTypeID, TemplateDataType> myTemplateDataTypes;

		inline static const ScriptColor mySelectionTint = ScriptColor(0.2f, 0.2f, 0.2f, 0);
		inline static const ScriptColor myHoverTint = ScriptColor(0.1f, 0.1f, 0.1f, 0);

		inline static const std::string myNullNameStr;

	};


	template<Scriptable<nlohmann::json> T>
	inline void DataTypeManager::Register(const std::string& aName, const ScriptColor& aColor)
	{
		RegisterInternal<T>(aName, aColor, CreateDataTypeInterface<T, true>());
	}

	template<Fundamental T>
	inline void DataTypeManager::Register(const std::string& aName, const ScriptColor& aColor)
	{
		RegisterInternal<T>(aName, aColor, CreateDataTypeInterface<T, true>());
	}

	template<typename T>
	inline void DataTypeManager::RegisterNonSerializableType(const std::string& aName, const ScriptColor& aColor)
	{
		RegisterInternal<T>(aName, aColor, CreateDataTypeInterface<T, false>());
	}

	template<template<typename> typename TemplateType>
	inline void DataTypeManager::RegisterTemplateType(const std::string& aName)
	{
		TemplateDataType templateObjectType
		{
			aName
		};
		myTemplateDataTypes.emplace(typeid(TemplateType).hash_code(), templateObjectType);
	}

	template<typename T>
	inline void DataTypeManager::RegisterTemplateSpecification(const std::string& aName, const ScriptColor& aColor)
	{
		DataTypeInterface dataTypeInterface
		{
			{
				CreateEditTemplateInterface<T>(),
				CreateSaveTemplateInterface<T>(),
				CreateLoadTemplateInterface<T>()
			},
			{
				CreateAllocateInterface<T>(),
				CreateCopyInterface<T>()
			}
		};
		RegisterInternal<T>(aName, aColor, dataTypeInterface);
	}

	template<CleanType T>
	inline void DataTypeManager::RegisterInternal(const std::string& aName, const ScriptColor& aColor, const DataTypeInterface& anInterface)
	{

		const std::type_info& typeInfo = typeid(T);
		DataType dataType
		{
			aName,
			aColor,
			&typeInfo,
			anInterface
		};

		if (myDataTypes.contains(typeInfo.hash_code()))
		{
			throw std::runtime_error("Two object types have the same hash value");
		}
		myDataTypes.emplace(typeInfo.hash_code(), dataType);
	}
}