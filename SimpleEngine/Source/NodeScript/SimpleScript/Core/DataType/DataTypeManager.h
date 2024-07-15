#pragma once
#include "../ScriptDefines.h"
#include "../Utilities/MetaScript.h"
#include "../Utilities/ScriptUtilities.h"
#include "../Memory/ScriptMemoryManager.h"
#include <nlohmann/json.hpp>

namespace SCR
{
	template<typename T>
	concept Fundamental = std::is_fundamental_v<T>;

	using EditInterface = bool(*)(void* aDataPtr);
	using SaveInterface = void(*)(nlohmann::json& aSaveObject, const void* aDataPtr);
	using LoadInterface = void(*)(const nlohmann::json& aLoadObject, void* aDataPtr);
	using AllocateInterface = void (*)(void* aDataPtr, const void* aDefaultValue);
	using ReleaseInterface = void (*)(void* aDataPtr);
	using CopyInterface = void(*)(void* aDestination, const void* aSource);
	using SwapInterface = void(*)(void* aDataPtr1, void* aDataPtr2);

	template<Editable T>
	constexpr EditInterface CreateEditInterface()
	{
		return [](void* aDataPtr) -> bool
			{
				T& value = *reinterpret_cast<T*>(aDataPtr);
				return Edit(value);
			};
	}

	template<Savable<nlohmann::json> T>
	constexpr SaveInterface CreateSaveInterface()
	{
		return [](nlohmann::json& aJson, const void* aDataPtr) -> void
			{
				const T& value = *reinterpret_cast<const T*>(aDataPtr);
				Save(aJson, value);
			};
	}

	template<Loadable<nlohmann::json> T>
	constexpr LoadInterface CreateLoadInterface()
	{
		return [](const nlohmann::json& aJson, void* aDataPtr) -> void
			{
				T& value = *reinterpret_cast<T*>(aDataPtr);
				Load(aJson, value);
			};
	}

	template<typename T>
	constexpr EditInterface CreateEditTemplateInterface()
	{
		return [](void* aDataPtr) -> bool
			{
				T& value = *reinterpret_cast<T*>(aDataPtr);
				return EditTemplate(value);
			};
	}

	template<typename T>
	constexpr SaveInterface CreateSaveTemplateInterface()
	{
		return [](nlohmann::json& aJson, const void* aDataPtr) -> void
			{
				const T& value = *reinterpret_cast<const T*>(aDataPtr);
				SaveTemplate(aJson, value);
			};
	}

	template<typename T>
	constexpr LoadInterface CreateLoadTemplateInterface()
	{
		return [](const nlohmann::json& aJson, void* aDataPtr) -> void
			{
				T& value = *reinterpret_cast<T*>(aDataPtr);
				LoadTemplate(aJson, value);
			};
	}

	template<Fundamental T>
	constexpr EditInterface CreateEditInterface()
	{
		return [](void* aDataPtr) -> bool
			{
				T& value = *reinterpret_cast<T*>(aDataPtr);
				return ::Edit(value);
			};
	}

	template<Fundamental T>
	constexpr SaveInterface CreateSaveInterface()
	{
		return [](nlohmann::json& aJson, const void* aDataPtr) -> void
			{
				const T& value = *reinterpret_cast<const T*>(aDataPtr);
				::Save(aJson, value);
			};
	}

	template<Fundamental T>
	constexpr LoadInterface CreateLoadInterface()
	{
		return [](const nlohmann::json& aJson, void* aDataPtr) -> void
			{
				T& value = *reinterpret_cast<T*>(aDataPtr);
				::Load(aJson, value);
			};
	}

	template<typename T>
	constexpr EditInterface CreateEditInterface()
	{
		return nullptr;
	}

	template<typename T>
	constexpr SaveInterface CreateSaveInterface()
	{
		return nullptr;
	}

	template<typename T>
	constexpr LoadInterface CreateLoadInterface()
	{
		return nullptr;
	}

	template<DefaultConstructible T>
	constexpr AllocateInterface CreateAllocateInterface()
	{
		return [](void* aDataPtr, const void* aDefaultValue) -> void
			{
				if (aDefaultValue != nullptr)
				{
					const T& defaultValue = *reinterpret_cast<const T*>(aDefaultValue);
					new(aDataPtr)T(defaultValue);
				}
				else
				{
					new(aDataPtr)T();
				}
			};
	}

	template<typename T>
	constexpr ReleaseInterface CreateReleaseInterface()
	{
		return [](void* const aDataPtr) -> void
			{
				T& value = *reinterpret_cast<T*>(aDataPtr);
				value.~T();
			};
	}

	template<Copyable T>
	constexpr CopyInterface CreateCopyInterface()
	{
		return [](void* aDestination, const void* aSource)
			{
				T& destination = *reinterpret_cast<T*>(aDestination);
				const T& source = *reinterpret_cast<const T*>(aSource);
				destination = source;
			};
	}

	template<Copyable T>
	constexpr SwapInterface CreateSwapInterface()
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

	template<typename T>
	constexpr FunctionInterface CreateFunctionInterface()
	{
		return FunctionInterface
		{
			.edit = CreateEditInterface<T>(),
			.save = CreateSaveInterface<T>(),
			.load = CreateLoadInterface<T>()
		};
	}

	template<typename T>
	constexpr CreationInterface CreateCreationInterface()
	{
		return CreationInterface
		{
			.allocate = CreateAllocateInterface<T>(),
			.release = CreateReleaseInterface<T>(),
			.copy = CreateCopyInterface<T>(),
			.swap = CreateSwapInterface<T>()
		};
	}

	template<typename T>
	constexpr DataTypeInterface CreateDataTypeInterface()
	{
		return DataTypeInterface
		{
			.function = CreateFunctionInterface<T>(),
			.creation = CreateCreationInterface<T>()
		};
	}


	struct Property
	{
		const std::string name;
		const DataTypeID typeID;
		const size_t byteOffset;
	};

	enum class eDataTypeTrait
	{
		None = 0,
		Fundamental = 1 << 0,
		Editable = 1 << 1,
		SaveLoadable = 1 << 2,
		All = Fundamental | Editable | SaveLoadable
	};

	struct DataType
	{
		const std::string name;
		const size_t size;
		const Color color;
		const std::type_info* typeInfo;
		const eDataTypeTrait typeTraits;
		const DataTypeInterface typeInterface;
		std::vector<Property> properties;
		std::vector<NodeTypeID> functions;
	};

	struct TemplateDataType
	{
		const std::string name;
	};

	const Color DefaultColor = Color(1.f, 1.f, 0.3f);

	class DataTypeManager
	{
		friend class DataTypeRegistry;
	public:

		DataTypeManager();
		~DataTypeManager();

		bool EditData(DataTypeID aDataTypeID, void* aDataPtr);
		bool SaveData(DataTypeID aDataTypeID, nlohmann::json& aJson, const void* aDataPtr);
		bool LoadData(DataTypeID aDataTypeID, const nlohmann::json& aJson, void* aDataPtr);

		template<size_t BufferCapacity>
		void* AllocateData(DataTypeID aDataTypeID, MemoryArena<BufferCapacity>& anArena, const void* aDefaultValue = nullptr);

		void ReleaseData(DataTypeID aDataTypeID, void* aDataPtr);

		void CopyData(DataTypeID aDataTypeID, void* aDestination, const void* aSource);
		void SwapData(DataTypeID aDataTypeID, void* aDataPtr1, void* aDataPtr2);
		const std::string& GetName(DataTypeID aDataTypeID);

		DataTypeID GetDataTypeIDByName(const std::string& aName);

		const std::unordered_map<DataTypeID, DataType>& GetDataTypes();
		std::unordered_map<DataTypeID, const DataType*> GetFunctionDataTypes();
		std::unordered_map<DataTypeID, const DataType*> GetDataTypesFiltered(eDataTypeTrait aTrait, eBitwiseType aBitwiseType);

		Color GetColor(const DataTypeID aDataTypeID);
		Color GetSelectionColor(const DataTypeID aDataTypeID);
		Color GetHoverColor(const DataTypeID aDataTypeID);

		DataType* Find(DataTypeID anID);

		template<typename T>
		DataType* Find();


		template<typename T>
		void Register(const std::string& aName, const Color& aColor = DefaultColor);

	private:

		template<template<typename> typename TemplateType>
		void RegisterTemplateType(const std::string& aName);

		template<typename T>
		void RegisterTemplateSpecification(const std::string& aName, const Color& aColor = DefaultColor);

		template<CleanType T>
		void RegisterInternal(const std::string& aName, const Color& aColor, const DataTypeInterface& anInterface);

		template<CleanType ClassType, CleanType PropertyType>
		void RegisterProperty(PropertyType ClassType::* aProperty, const std::string& aName);

		template<CleanType T>
		bool HasRegisteredType() const;

	private:

		std::unordered_map<DataTypeID, DataType> myDataTypes;
		std::unordered_map<DataTypeID, TemplateDataType> myTemplateDataTypes;

		const Color mySelectionTint = Color(0.2f, 0.2f, 0.2f, 0);
		const Color myHoverTint = Color(0.1f, 0.1f, 0.1f, 0);

		const std::string myNullNameStr;
	};

	template<typename T>
	inline void DataTypeManager::Register(const std::string& aName, const Color& aColor)
	{
		RegisterInternal<T>(aName, aColor, CreateDataTypeInterface<T>());
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
	inline void DataTypeManager::RegisterTemplateSpecification(const std::string& aName, const Color& aColor)
	{
		DataTypeInterface dataTypeInterface
		{
			.function = FunctionInterface
			{
				.edit = CreateEditTemplateInterface<T>(),
				.save = CreateSaveTemplateInterface<T>(),
				.load = CreateLoadTemplateInterface<T>()
			},
			.creation = CreationInterface
			{
				.allocate = CreateAllocateInterface<T>(),
				.copy = CreateCopyInterface<T>(),
			}
		};
		RegisterInternal<T>(aName, aColor, dataTypeInterface);
	}

	template<CleanType T>
	inline void DataTypeManager::RegisterInternal(const std::string& aName, const Color& aColor, const DataTypeInterface& anInterface)
	{
		eDataTypeTrait typeTraits = eDataTypeTrait::None;
		if constexpr (Fundamental<T>)
		{
			typeTraits |= eDataTypeTrait::Fundamental;
		}
		if constexpr (Editable<T>)
		{
			typeTraits |= eDataTypeTrait::Editable;
		}
		if constexpr (Loadable<T, nlohmann::json> && Savable<T, nlohmann::json>)
		{
			typeTraits |= eDataTypeTrait::SaveLoadable;
		}

		const std::type_info& typeInfo = typeid(T);
		DataType dataType
		{
			.name = aName,
			.size = sizeof(T),
			.color = aColor,
			.typeInfo = &typeInfo,
			.typeTraits = typeTraits,
			.typeInterface = anInterface,
		};

		myDataTypes.emplace(typeInfo.hash_code(), dataType);
		/*auto [it, success] = myDataTypes.emplace(typeInfo.hash_code(), dataType);
		if (!success)
		{
			throw std::runtime_error("Two data types have the same hash value");
		}*/
	}

	template<CleanType ClassType, CleanType PropertyType>
	inline void DataTypeManager::RegisterProperty(PropertyType ClassType::* aProperty, const std::string& aName)
	{
		size_t byteOffset = GetByteOffset(aProperty);

		DataTypeID dataTypeID = typeid(PropertyType).hash_code();

		Property property
		{
			.name = aName,
			.typeID = dataTypeID,
			.byteOffset = byteOffset
		};

		if (DataType* classDataType = Find<ClassType>())
		{
			classDataType->properties.push_back(property);
		}
	}

	template<CleanType T>
	inline bool DataTypeManager::HasRegisteredType() const
	{
		const DataTypeID dataTypeID = GetDataTypeID<T>();
		return myDataTypes.find(dataTypeID) != myDataTypes.end();
	}

	template<size_t BufferCapacity>
	inline void* DataTypeManager::AllocateData(const DataTypeID aDataTypeID, MemoryArena<BufferCapacity>& anArena, const void* const aDefaultValue)
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			if (dataType->typeInterface.creation.allocate)
			{
				void* dataPtr = anArena.AllocateSize(dataType->size);
				dataType->typeInterface.creation.allocate(dataPtr, aDefaultValue);

				return dataPtr;
			}
		}
		return nullptr;
	}

	template<typename T>
	inline DataType* DataTypeManager::Find()
	{
		return Find(typeid(T).hash_code());
	}
}