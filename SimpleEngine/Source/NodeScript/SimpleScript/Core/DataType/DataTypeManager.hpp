#pragma once
#include "../ScriptDefines.hpp"
#include "../Utilities/MetaScript.hpp"
#include "../Utilities/ScriptUtilities.hpp"
#include "../Memory/ScriptMemoryArena.hpp"
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
		const std::string mName;
		const DataTypeID mTypeID;
		const size_t byteOffset;
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
		std::vector<NodeTypeID> mFunctions;
	};

	struct TemplateDataType
	{
		const std::string mName;
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

		Color GetColor(DataTypeID aDataTypeID);
		Color GetSelectionColor(DataTypeID aDataTypeID);
		Color GetHoverColor(DataTypeID aDataTypeID);

		DataType* Find(DataTypeID anID);

		template<typename T>
		DataType* Find();


		template<typename T>
		void Register(const std::string& aName, const Color& aColor, bool aIsTargetable);

	private:

		template<template<typename> typename TemplateType>
		void RegisterTemplateType(const std::string& aName);

		template<typename T>
		void RegisterTemplateSpecification(const std::string& aName, const Color& aColor = DefaultColor);

		template<CleanType T>
		void RegisterInternal(const std::string& aName, const Color& aColor, const DataTypeInterface& anInterface, bool aIsTargetable);

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
	inline void DataTypeManager::Register(const std::string& aName, const Color& aColor, const bool aIsTargetable)
	{
		RegisterInternal<T>(aName, aColor, CreateDataTypeInterface<T>(), aIsTargetable);
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
	inline void DataTypeManager::RegisterInternal(const std::string& aName, const Color& aColor, const DataTypeInterface& anInterface, const bool aIsTargetable)
	{
		eDataTypeTrait mTypeTraits = eDataTypeTrait::None;
		if constexpr (Fundamental<T>)
		{
			mTypeTraits |= eDataTypeTrait::Fundamental;
		}
		if constexpr (Editable<T>)
		{
			mTypeTraits |= eDataTypeTrait::Editable;
		}
		if constexpr (Loadable<T, nlohmann::json> && Savable<T, nlohmann::json>)
		{
			mTypeTraits |= eDataTypeTrait::SaveLoadable;
		}
		if (aIsTargetable)
		{
			mTypeTraits |= eDataTypeTrait::Targetable;
		}

		const std::type_info& mTypeInfo = typeid(T);
		DataType dataType
		{
			.mName = aName,
			.mSize = sizeof(T),
			.mColor = aColor,
			.mTypeInfo = &mTypeInfo,
			.mTypeTraits = mTypeTraits,
			.mInterface = anInterface,
		};

		myDataTypes.emplace(mTypeInfo.hash_code(), dataType);
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
			.mName = aName,
			.mTypeID = dataTypeID,
			.byteOffset = byteOffset
		};

		if (DataType* classDataType = Find<ClassType>())
		{
			classDataType->mProperties.push_back(property);
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
			if (dataType->mInterface.creation.allocate)
			{
				void* mDataPtr = anArena.AllocateSize(dataType->mSize);
				dataType->mInterface.creation.allocate(mDataPtr, aDefaultValue);

				return mDataPtr;
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