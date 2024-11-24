#pragma once
#include "../FlyDefines.hpp"
#include "../Utilities/FlyMeta.hpp"
#include "../Utilities/FlyUtilities.hpp"
#include "../Memory/FlyMemoryArena.hpp"
#include "FlyDataType.hpp"
#include "../Pin/FlyPinType.hpp"
#include <nlohmann/json.hpp>

namespace FLY_NAMESPACE
{
	template<typename T>
	concept Fundamental = std::is_fundamental_v<T>;


	template<typename T>
	constexpr AllocateInterface CreateAllocateInterface()
	{
		if constexpr (DefaultConstructible<T>)
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
		else
		{
			return [](void*, const void*) -> void
				{
					throw std::runtime_error("Can't instantiate an object that isn't default constructible.");
				};
		}
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

	template<std::swappable T>
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
	constexpr EqualsInterface CreateEqualsInterface()
	{
		if constexpr (HasOperator_EqualTo<T>)
		{
			return [](const void* aDataPtr1, const void* aDataPtr2) -> bool
				{
					const T& value1 = *reinterpret_cast<const T*>(aDataPtr1);
					const T& value2 = *reinterpret_cast<const T*>(aDataPtr2);

					return value1 == value2;
				};
		}
		else
		{
			return nullptr;
		}
	}

	template<typename T>
	constexpr ViewAndEditInterface CreateEditInterface()
	{
		if constexpr (ViewAndEditable<T>)
		{
			return [](void* aDataPtr) -> eIsItemActive
				{
					T& value = *reinterpret_cast<T*>(aDataPtr);
					return ViewAndEdit(value);
				};
		}
		else if constexpr (Fundamental<T>)
		{
			return [](void* aDataPtr) -> eIsItemActive
				{
					T& value = *reinterpret_cast<T*>(aDataPtr);
					return ::ViewAndEdit(value);
				};
		}
		else
		{
			return nullptr;
		}
	}

	template<typename T>
	constexpr SaveInterface CreateSaveInterface()
	{
		if constexpr (Savable<T, nlohamnn::json>)
		{
			return [](nlohmann::json& aJson, const void* aDataPtr) -> void
				{
					const T& value = *reinterpret_cast<const T*>(aDataPtr);
					Save(value, aJson);
				};
		}
		else if constexpr (Fundamental<T>)
		{
			return [](nlohmann::json& aJson, const void* aDataPtr) -> void
				{
					const T& value = *reinterpret_cast<const T*>(aDataPtr);
					::Save(value, aJson);
				};
		}
		else
		{
			return nullptr;
		}

	}

	template<typename T>
	constexpr LoadInterface CreateLoadInterface()
	{
		if constexpr (Loadable<T, nlohmann::json>)
		{
			return [](const nlohmann::json& aJson, void* aDataPtr) -> void
				{
					T& value = *reinterpret_cast<T*>(aDataPtr);
					Load(value, aJson);
				};
		}
		else if constexpr (Fundamental<T>)
		{
			return [](const nlohmann::json& aJson, void* aDataPtr) -> void
				{
					T& value = *reinterpret_cast<T*>(aDataPtr);
					::Load(value, aJson);
				};
		}
		else
		{
			return nullptr;
		}
	}

	template<typename T>
	constexpr ViewAndEditInterface CreateEditTemplateInterface()
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

	template<typename T>
	constexpr FundamentalInterface CreateFundamentalInterface()
	{
		return FundamentalInterface
		{
			.allocate = CreateAllocateInterface<T>(),
			.release = CreateReleaseInterface<T>(),
			.copy = CreateCopyInterface<T>(),
			.swap = CreateSwapInterface<T>(),
			.equals = CreateEqualsInterface<T>()
		};
	}

	template<typename T>
	constexpr FunctionInterface CreateFunctionInterface()
	{
		return FunctionInterface
		{
			.viewAndEdit = CreateEditInterface<T>(),
			.save = CreateSaveInterface<T>(),
			.load = CreateLoadInterface<T>()
		};
	}

	

	template<typename T>
	constexpr ExecutionInterface CreateExecutionInterface()
	{
		return ExecutionInterface
		{
			.setInputPinData = CreateSetPinDataInterface<T, eFlowType::Input>(),
			.setOutputPinData = CreateSetPinDataInterface<T, eFlowType::Output>()
		};
	}

	template<typename T>
	constexpr DataTypeInterface CreateDataTypeInterface()
	{
		return DataTypeInterface
		{
			.fundamental = CreateFundamentalInterface<T>(),
			.function = CreateFunctionInterface<T>(),
			.execution = CreateExecutionInterface<T>()
		};
	}

	constexpr Color DefaultColor = Color(1.f, 1.f, 0.3f);

	class DataTypeManager final
	{
		friend class DataTypeRegistry;
	public:

		DataTypeManager();
		~DataTypeManager();

	private:

		eIsItemActive ViewAndEditData(const DataType& aDataType, void* aDataPtr) const;
		bool SaveData(const DataType& aDataType, nlohmann::json& aJson, const void* aDataPtr) const;
		bool LoadData(const DataType& aDataType, const nlohmann::json& aJson, void* aDataPtr) const;

	public:

		eIsItemActive ViewAndEditData(DataTypeID aDataTypeID, void* aDataPtr) const;
		bool SaveData(DataTypeID aDataTypeID, nlohmann::json& aJson, const void* aDataPtr) const;
		bool LoadData(DataTypeID aDataTypeID, const nlohmann::json& aJson, void* aDataPtr) const;

		template<size_t BufferCapacity>
		void* AllocateData(DataTypeID aDataTypeID, MemoryArena<BufferCapacity>& anArena, const void* aDefaultValue = nullptr) const;

		void ReleaseData(DataTypeID aDataTypeID, void* aDataPtr) const;

		void CopyData(DataTypeID aDataTypeID, void* aDestination, const void* aSource) const;
		void SwapData(DataTypeID aDataTypeID, void* aDataPtr1, void* aDataPtr2) const;
		bool DataEqualsTo(DataTypeID aDataTypeID, const void* aDataPtr1, const void* aDataPtr2) const;

		const std::string& GetName(DataTypeID aDataTypeID) const;

		SetPinDataInterface GetSetPinDataInterface(DataTypeID aDataTypeID, eFlowType aFlowType) const;

		DataTypeID GetDataTypeIDByName(const std::string& aName) const;

		const std::unordered_map<DataTypeID, DataType>& GetDataTypes() const;

		DataType* Find(DataTypeID aDataTypeID);
		const DataType* Find(DataTypeID aDataTypeID) const;

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

		template<Decayed T>
		void RegisterInternal(const std::string& aName, const Color& aColor, const DataTypeInterface& anInterface, bool aIsTargetable);

		template<Decayed ClassType, Decayed MemberType>
		void RegisterMemberVariable(MemberType ClassType::* aMemberVariable, const std::string& aName);

		template<Decayed T>
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
			.fundamental = FundamentalInterface
			{
				.allocate = CreateAllocateInterface<T>(),
				.copy = CreateCopyInterface<T>(),
			}
		};
		RegisterInternal<T>(aName, aColor, dataTypeInterface);
	}

	template<Decayed T>
	inline void DataTypeManager::RegisterInternal(const std::string& aName, const Color& aColor, const DataTypeInterface& anInterface, const bool aIsTargetable)
	{
		eDataTypeTrait typeTraits = eDataTypeTrait::None;
		if constexpr (Fundamental<T>)
		{
			typeTraits |= eDataTypeTrait::Fundamental;
		}
		if (anInterface.function.viewAndEdit)
		{
			typeTraits |= eDataTypeTrait::ViewAndEditable;
		}
		if (anInterface.function.save && anInterface.function.load)
		{
			typeTraits |= eDataTypeTrait::SaveLoadable;
		}
		if (aIsTargetable)
		{
			typeTraits |= eDataTypeTrait::Targetable;
		}
		if constexpr (IsPointer<T>)
		{
			typeTraits |= eDataTypeTrait::Pointer;
		}

		const std::type_info& typeInfo = typeid(T);
		DataType dataType
		{
			.mName = aName,
			.mSize = sizeof(T),
			.mColor = aColor,
			.mTypeInfo = &typeInfo,
			.mTypeTraits = typeTraits,
			.mInterface = anInterface,
		};

		auto [it, success] = mDataTypes.emplace(typeInfo.hash_code(), dataType);
		if (!success && aName != it->second.mName)
		{
			throw std::runtime_error("Two data types have the same hash value");
		}
	}

	template<Decayed ClassType, Decayed MemberType>
	inline void DataTypeManager::RegisterMemberVariable(MemberType ClassType::* aMemberVariable, const std::string& aName)
	{
		const size_t byteOffset = GetByteOffset(aMemberVariable);

		const DataTypeID dataTypeID = GetDataTypeID<MemberType>();

		Variable variable
		{
			.mDataTypeID = dataTypeID,
			.mName = aName,
			.mByteOffset = byteOffset
		};

		DataType* parentDataType = Find<ClassType*>();

		if (parentDataType)
		{
			parentDataType->mVariables.push_back(variable);
		}
	}

	template<Decayed T>
	inline bool DataTypeManager::HasRegisteredType() const
	{
		return mDataTypes.contains(GetDataTypeID<T>());
	}

	template<size_t BufferCapacity>
	inline void* DataTypeManager::AllocateData(const DataTypeID aDataTypeID, MemoryArena<BufferCapacity>& anArena, const void* const aDefaultValue) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			if (dataType->mInterface.fundamental.allocate)
			{
				void* dataPtr = anArena.AllocateSize(dataType->mSize);
				dataType->mInterface.fundamental.allocate(dataPtr, aDefaultValue);

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
		return Find(GetDataTypeID<T>());
	}

	template<typename T>
	inline const DataType* DataTypeManager::Find() const
	{
		return Find(GetDataTypeID<T>());
	}
}