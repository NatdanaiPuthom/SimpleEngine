#pragma once
#include "../FlyDefines.hpp"
#include "../Utilities/FlyMeta.hpp"
#include "../Utilities/FlyUtilities.hpp"
#include "../Memory/FlyMemoryArena.hpp"
#include "FlyDataType.hpp"
#include <nlohmann/json.hpp>

namespace FLY_NAMESPACE
{
	template<typename T>
	concept Fundamental = std::is_fundamental_v<T>;

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

	template<NotDefaultConstructible T>
	constexpr AllocateInterface CreateAllocateInterface()
	{
		return [](void*, const void*) -> void
			{
				throw std::runtime_error("Can't instantiate an object that isn't default constructible.");
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

	const Color DefaultColor = Color(1.f, 1.f, 0.3f);

	class DataTypeManager
	{
		friend class DataTypeRegistry;
	public:

		DataTypeManager();
		~DataTypeManager();
		
	private:

		bool EditData(const DataType& aDataType, void* aDataPtr) const;
		bool SaveData(const DataType& aDataType, nlohmann::json& aJson, const void* aDataPtr) const;
		bool LoadData(const DataType& aDataType, const nlohmann::json& aJson, void* aDataPtr) const;

	public:

		bool EditData(DataTypeID aDataTypeID, void* aDataPtr) const;
		bool SaveData(DataTypeID aDataTypeID, nlohmann::json& aJson, const void* aDataPtr) const;
		bool LoadData(DataTypeID aDataTypeID, const nlohmann::json& aJson, void* aDataPtr) const;

		template<size_t BufferCapacity>
		void* AllocateData(DataTypeID aDataTypeID, MemoryArena<BufferCapacity>& anArena, const void* aDefaultValue = nullptr) const;

		void ReleaseData(DataTypeID aDataTypeID, void* aDataPtr) const;

		void CopyData(DataTypeID aDataTypeID, void* aDestination, const void* aSource) const;
		void SwapData(DataTypeID aDataTypeID, void* aDataPtr1, void* aDataPtr2) const;
		const std::string& GetName(DataTypeID aDataTypeID) const;

		DataTypeID GetDataTypeIDByName(const std::string& aName) const;

		const std::unordered_map<DataTypeID, DataType>& GetDataTypes() const;

		DataType* Find(DataTypeID anID);
		const DataType* Find(DataTypeID anID) const;

		template<typename T>
		DataType* Find();

		template<typename T>
		const DataType* Find() const;


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

		std::unordered_map<DataTypeID, DataType> mDataTypes;
		std::unordered_map<DataTypeID, TemplateDataType> mTemplateDataTypes;

		

		const std::string mNullNameStr;
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
		mTemplateDataTypes.emplace(typeid(TemplateType).hash_code(), templateObjectType);
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
		if (anInterface.function.edit)
		{
			mTypeTraits |= eDataTypeTrait::Editable;
		}
		if (anInterface.function.save && anInterface.function.load)
		{
			mTypeTraits |= eDataTypeTrait::SaveLoadable;
		}
		if (aIsTargetable)
		{
			mTypeTraits |= eDataTypeTrait::Targetable;
		}

		const std::type_info& typeInfo = typeid(T);
		DataType dataType
		{
			.mName = aName,
			.mSize = sizeof(T),
			.mColor = aColor,
			.mTypeInfo = &typeInfo,
			.mTypeTraits = mTypeTraits,
			.mInterface = anInterface,
		};

		mDataTypes.emplace(typeInfo.hash_code(), dataType);
		/*auto [it, success] = myDataTypes.emplace(typeInfo.hash_code(), dataType);
		if (!success)
		{
			throw std::runtime_error("Two data types have the same hash value");
		}*/
	}

	template<CleanType ClassType, CleanType PropertyType>
	inline void DataTypeManager::RegisterProperty(PropertyType ClassType::* aProperty, const std::string& aName)
	{
		const size_t byteOffset = GetByteOffset(aProperty);

		const DataTypeID dataTypeID = typeid(PropertyType).hash_code();

		Variable variable
		{
			.mDataTypeID = dataTypeID,
			.mName = aName,
			.mByteOffset = byteOffset
		};

		if (DataType* classDataType = Find<ClassType>())
		{
			classDataType->mVariables.push_back(variable);
		}
	}

	template<CleanType T>
	inline bool DataTypeManager::HasRegisteredType() const
	{
		const DataTypeID dataTypeID = GetDataTypeID<T>();
		return mDataTypes.find(dataTypeID) != mDataTypes.end();
	}

	template<size_t BufferCapacity>
	inline void* DataTypeManager::AllocateData(const DataTypeID aDataTypeID, MemoryArena<BufferCapacity>& anArena, const void* const aDefaultValue) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			if (dataType->mInterface.creation.allocate)
			{
				void* dataPtr = anArena.AllocateSize(dataType->mSize);
				dataType->mInterface.creation.allocate(dataPtr, aDefaultValue);

				if (HasNotFlag(dataType->mTypeTraits, eDataTypeTrait::Fundamental))
				{
					anArena.RegisterMemoryObject(dataPtr, aDataTypeID);
				}

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

	template<typename T>
	inline const DataType* DataTypeManager::Find() const
	{
		return Find(typeid(T).hash_code());
	}
}