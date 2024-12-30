#pragma once
#include "../FlyDefines.hpp"
#include "FlyDataType.hpp"
#include "FlyStruct.hpp"
#include "../Instance/FlyStructInstance.hpp"
#include "FlyClass.hpp"
#include "../Instance/FlyClassInstance.hpp"
#include "../Utilities/FlyMeta.hpp"
#include "../Utilities/FlyUtilities.hpp"
#include "../Memory/FlyMemoryArena.hpp"
#include "../Pin/FlyPinType.hpp"
#include <nlohmann/json.hpp>

namespace FLY_NAMESPACE
{
	template<typename T>
	concept Fundamental = std::is_fundamental_v<T>;


	inline void(*editorNullptrFunction)();


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

	// Primary template: assumes T is not a specialization of the given class template
	template <template <typename...> class Template, typename T>
	struct is_specialization_of : std::false_type {};

	// Specialization: detects if T is a specialization of the given class template
	template <template <typename...> class Template, typename... Args>
	struct is_specialization_of<Template, Template<Args...>> : std::true_type {};

	// Helper variable template for convenience
	template <template <typename...> class Template, typename T>
	constexpr bool is_specialization_of_v = is_specialization_of<Template, T>::value;

	//template<typename T>
	//void DoSomething()
	//{
	//	if constexpr (/*T Is std::vector*/)
	//	{
	//		// Do something
	//	}
	//}

	template<typename T>
	constexpr EqualsInterface CreateEqualsInterface()
	{
		if constexpr (is_specialization_of_v<std::vector, T>)
		{
			return nullptr;
		}
		else
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
	}

	template<typename T>
	constexpr ViewAndEditInterface CreateViewAndEditInterface()
	{
		if constexpr (ViewAndEditable<T>)
		{
			return [](void* aDataPtr) -> ViewAndEditResult
				{
					T& value = *reinterpret_cast<T*>(aDataPtr);
					return ViewAndEdit(value);
				};
		}
		else if constexpr (PointerType<T>)
		{
			if constexpr (ViewAndEditable<std::remove_pointer_t<T>>)
			{
				return nullptr;
			}
			else
			{
				return [](void*) -> ViewAndEditResult
					{
						//T& value = *reinterpret_cast<T*>(aDataPtr);
						//if (value)
						//{
						//	//return ViewAndEdit(*value);
						//}
						//else if (editorNullptrFunction)
						//{
						editorNullptrFunction();
						//}

						return ViewAndEditResult{};
					};
			}
		}
		else if constexpr (Fundamental<T>)
		{
			return [](void* aDataPtr) -> ViewAndEditResult
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
	constexpr ViewInterface CreateViewInterface()
	{
		if constexpr (Viewable<T>)
		{
			return [](const void* aDataPtr) -> void
				{
					const T& value = *reinterpret_cast<const T*>(aDataPtr);
					View(value);
				};
		}
		else if constexpr (PointerType<T>)
		{
			if constexpr (Viewable<std::remove_pointer_t<T>>)
			{
				return [](const void* aDataPtr) -> void
					{
						const T& value = *reinterpret_cast<const T*>(aDataPtr);

						if (value)
						{
							View(*value);
						}
						else if (editorNullptrFunction)
						{
							editorNullptrFunction();
						}
					};
			}
			else if constexpr (Fundamental<std::remove_pointer_t<T>>)
			{
				if constexpr (GlobalViewable<std::remove_pointer_t<T>>)
				{
					return [](const void* aDataPtr) -> void
						{
							const T& value = *reinterpret_cast<const T*>(aDataPtr);

							if (value)
							{
								::View(*value);
							}
							else if (editorNullptrFunction)
							{
								editorNullptrFunction();
							}
						};
				}
				else
				{
					return nullptr;
				}
			}
			else
			{
				return nullptr;
			}

		}
		else if constexpr (Fundamental<T> and GlobalViewable<T>)
		{
			return [](const void* aDataPtr) -> void
				{
					const T& value = *reinterpret_cast<const T*>(aDataPtr);

					::View(value);
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
		if constexpr (Savable<T, nlohmann::json>)
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
			.viewAndEdit = CreateViewAndEditInterface<T>(),
			.view = CreateViewInterface<T>(),
			.save = CreateSaveInterface<T>(),
			.load = CreateLoadInterface<T>()
		};
	}



	template<typename T>
	constexpr ExecutionInterface CreateExecutionInterface()
	{
		return ExecutionInterface
		{
			.setInputPinValue = CreateSetPinValueInterface<T, eFlowType::Input>(),
			.setOutputPinValue = CreateSetPinValueInterface<T, eFlowType::Output>(),
			.setInputPinValueFromPin = CreateSetPinValueFromPinInterface<T, eFlowType::Input>(),
			.setOutputPinValueFromPin = CreateSetPinValueFromPinInterface<T, eFlowType::Output>(),
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



	struct FlyCustomVectorType;

	using GenericDataTypePtr = std::variant<DataType*, Struct*, Class*>;

	class DataTypeManager final
	{
		friend class DataTypeRegistry;
	public:

		DataTypeManager();
		~DataTypeManager();

	private:

		[[nodiscard]] ViewAndEditResult ViewAndEditData(const DataType& aDataType, void* aDataPtr) const;
		void ViewData(const DataType& aDataType, const void* aDataPtr) const;
		bool SaveData(const DataType& aDataType, const void* aDataPtr, nlohmann::json& aJson) const;
		bool LoadData(const DataType& aDataType, void* aDataPtr, const nlohmann::json& aJson) const;

	public:

		[[nodiscard]] ViewAndEditResult ViewAndEditData(DataTypeID aDataTypeID, void* aDataPtr) const;
		void ViewData(DataTypeID aDataTypeID, const void* aDataPtr) const;
		bool SaveData(DataTypeID aDataTypeID, const void* aDataPtr, nlohmann::json& aJson) const;
		bool LoadData(DataTypeID aDataTypeID, void* aDataPtr, const nlohmann::json& aJson) const;

		template<size_t BufferCapacity>
		[[nodiscard]] void* AllocateData(DataTypeID aDataTypeID, MemoryArena<BufferCapacity>& aArena, const void* aDefaultValue = nullptr) const;

		template<size_t BufferCapacity>
		[[nodiscard]] StructInstance* AllocateStructInstance(StructID aStructID, MemoryArena<BufferCapacity>& aArena) const;

		template<size_t BufferCapacity>
		[[nodiscard]] ClassInstance* AllocateClassInstance(ClassID aClassID, MemoryArena<BufferCapacity>& aArena) const;


		void ReleaseData(DataTypeID aDataTypeID, void* aDataPtr) const;

		void CopyData(DataTypeID aDataTypeID, void* aDestination, const void* aSource) const;
		void SwapData(DataTypeID aDataTypeID, void* aDataPtr1, void* aDataPtr2) const;
		[[nodiscard]] bool DataEqualsTo(DataTypeID aDataTypeID, const void* aDataPtr1, const void* aDataPtr2) const;


		[[nodiscard]] ViewAndEditResult ViewAndEditData(GenericDataTypeID aDataTypeID, void* aDataPtr) const;
		void ViewData(GenericDataTypeID aDataTypeID, const void* aDataPtr) const;
		bool SaveData(GenericDataTypeID aDataTypeID, const void* aDataPtr, nlohmann::json& aJson) const;
		bool LoadData(GenericDataTypeID aDataTypeID, void* aDataPtr, const nlohmann::json& aJson) const;

		template<size_t BufferCapacity>
		[[nodiscard]] void* AllocateData(GenericDataTypeID aDataTypeID, MemoryArena<BufferCapacity>& aArena, const void* aDefaultValue = nullptr) const;
		void ReleaseData(GenericDataTypeID aDataTypeID, void* aDataPtr) const;

		void CopyData(GenericDataTypeID aDataTypeID, void* aDestination, const void* aSource) const;
		void SwapData(GenericDataTypeID aDataTypeID, void* aDataPtr1, void* aDataPtr2) const;

		[[nodiscard]] bool AreDataTypesRelated(DataTypeID aDataTypeID1, DataTypeID aDataTypeID2) const;
		[[nodiscard]] bool AreDataTypesRelated(GenericDataTypeID aDataTypeID1, GenericDataTypeID aDataTypeID2) const;
		[[nodiscard]] eDataTypeRelation GetDataTypeRelation(DataTypeID aDataTypeID1, DataTypeID aDataTypeID2) const;
		[[nodiscard]] eDataTypeRelation GetDataTypeRelation(GenericDataTypeID aDataTypeID1, GenericDataTypeID aDataTypeID2) const;

		[[nodiscard]] const std::string& GetName(DataTypeID aDataTypeID) const;
		[[nodiscard]] std::string_view GetName(GenericDataTypeID aDataTypeID) const;

		[[nodiscard]] SetPinValueInterface GetSetPinValueInterface(DataTypeID aDataTypeID, eFlowType aFlowType) const;
		[[nodiscard]] SetPinValueFromPinInterface GetSetPinValueFromPinInterface(DataTypeID aDataTypeID, eFlowType aFlowType) const;

		[[nodiscard]] DataTypeID GetDataTypeIDByName(const std::string& aName) const;

		void SetEditorNullptrFunction(void(*aFunction)());

		[[nodiscard]] const std::unordered_map<DataTypeID, DataType>& GetDataTypes() const;

		[[nodiscard]] DataType* Find(DataTypeID aDataTypeID);
		[[nodiscard]] const DataType* Find(DataTypeID aDataTypeID) const;

		template<typename T>
		[[nodiscard]] DataType* Find();

		template<typename T>
		[[nodiscard]] const DataType* Find() const;

		[[nodiscard]] GenericDataTypePtr Find(GenericDataTypeID aDataTypeID);

		template<Decayed T>
		bool IsRegistered() const;

		bool IsRegistered(DataTypeID aDataTypeID) const;

		void SetDefaultColor(const Color& aColor);

		[[nodiscard]] Color GetDefaultColor() const;

		template<typename T>
		void Register(const std::string& aName, const Color& aColor, bool aIsTargetable);

		template<typename T>
		void Register(const std::string& aName, bool aIsTargetable);


		StructID CreateStruct(std::string_view aName);
		[[nodiscard]] Struct& GetStruct(StructID aStructID);
		[[nodiscard]] const Struct& GetStruct(StructID aStructID) const;
		[[nodiscard]] StructID GetStructIDByName(std::string_view aName) const;
		[[nodiscard]] const std::vector<HeapObject<Struct>>& GetStructs() const;

		ClassID CreateClass(DataTypeID aTargetID, std::string_view aName);
		[[nodiscard]] Class& GetClass(ClassID aClassID);
		[[nodiscard]] const Class& GetClass(ClassID aClassID) const;
		[[nodiscard]] ClassID GetClassIDByName(std::string_view aName) const;
		[[nodiscard]] const std::vector<HeapObject<Class>>& GetClasses() const;

	private:

		template<template<typename> typename TemplateType>
		void RegisterTemplateType(const std::string& aName);

		template<typename T>
		void RegisterTemplateSpecification(const std::string& aName, const Color& aColor);

		template<typename T>
		void RegisterTemplateSpecification(const std::string& aName);

		template<Decayed T>
		void RegisterInternal(const std::string& aName, const Color& aColor, const DataTypeInterface& anInterface, bool aIsTargetable);

		template<Decayed ClassType, Decayed MemberType>
		void RegisterMemberVariable(MemberType ClassType::* aMemberVariable, const std::string& aName);

	private:

		std::unordered_map<DataTypeID, DataType> mDataTypes;
		std::unordered_map<DataTypeID, TemplateDataType> mTemplateDataTypes;
		std::vector<HeapObject<Struct>> mStructs;
		std::vector<HeapObject<Class>> mClasses;

		std::string mNullNameStr;
		Fly::Color mDefaultColor = Color(1.000f, 0.131f, 0.978f, 1.000f);
	};

	template<typename T>
	inline void DataTypeManager::Register(const std::string& aName, const Color& aColor, const bool aIsTargetable)
	{
		RegisterInternal<T>(aName, aColor, CreateDataTypeInterface<T>(), aIsTargetable);

		if constexpr (!PointerType<T>)
		{
			RegisterInternal<T*>(aName + " (Ptr)", aColor, CreateDataTypeInterface<T*>(), false);
		}
	}

	template<typename T>
	inline void DataTypeManager::Register(const std::string& aName, const bool aIsTargetable)
	{
		Register<T>(aName, mDefaultColor, aIsTargetable);
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

	template<typename T>
	inline void DataTypeManager::RegisterTemplateSpecification(const std::string& aName)
	{
		RegisterTemplateSpecification<T>(aName, mDefaultColor);
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
		if constexpr (PointerType<T>)
		{
			typeTraits |= eDataTypeTrait::Pointer;
		}
		if constexpr (std::is_trivially_copyable_v<T>)
		{
			typeTraits |= eDataTypeTrait::TriviallyCopyable;
		}

		const std::type_info& typeInfo = typeid(T);
		DataType dataType
		{
			.mName = aName,
			.mSize = sizeof(T),
			.mAlignment = alignof(T),
			.mColor = aColor,
			.mTypeInfo = &typeInfo,
			.mInterface = anInterface,
			.mToPointerDataTypeID = GetDataTypeID<T*>(),
			.mToValueDataTypeID = GetDataTypeID<std::remove_pointer_t<T>>(),
			.mTypeTraits = typeTraits,
		};

		auto [it, success] = mDataTypes.emplace(GetDataTypeID<T>(), dataType);
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

		DataType* const parentDataType = Find<ClassType>();

		if (parentDataType)
		{
			parentDataType->mVariables.push_back(
				Variable
				{
					.mDataTypeID = dataTypeID,
					.mName = aName,
					.mByteOffset = byteOffset
				}
			);
		}
	}

	template<Decayed T>
	inline bool DataTypeManager::IsRegistered() const
	{
		return IsRegistered(GetDataTypeID<T>());
	}

	template<size_t BufferCapacity>
	inline void* DataTypeManager::AllocateData(const DataTypeID aDataTypeID, MemoryArena<BufferCapacity>& aArena, const void* const aDefaultValue) const
	{
		if (const DataType* dataType = Find(aDataTypeID))
		{
			if (dataType->mInterface.fundamental.allocate)
			{
				void* dataPtr = aArena.AllocateSize(dataType->mSize);
				dataType->mInterface.fundamental.allocate(dataPtr, aDefaultValue);

				if (HasNotFlag(dataType->mTypeTraits, eDataTypeTrait::TriviallyCopyable))
				{
					aArena.RegisterMemoryObject(dataPtr, aDataTypeID);
				}

				return dataPtr;
			}
		}
		return nullptr;
	}

	template<size_t BufferCapacity>
	inline StructInstance* DataTypeManager::AllocateStructInstance(const StructID aStructID, MemoryArena<BufferCapacity>& aArena) const
	{
		const Struct& s = GetStruct(aStructID);
		return &aArena.Allocate<StructInstance>(s);
	}

	template<size_t BufferCapacity>
	inline ClassInstance* DataTypeManager::AllocateClassInstance(const ClassID aClassID, MemoryArena<BufferCapacity>& aArena) const
	{
		return &aArena.Allocate<ClassInstance>(aClassID);
	}

	template<size_t BufferCapacity>
	inline void* DataTypeManager::AllocateData(const GenericDataTypeID aDataTypeID, MemoryArena<BufferCapacity>& aArena, const void* aDefaultValue) const
	{
		return std::visit(Visitor{
			[this, &aArena, aDefaultValue](const DataTypeID aDataTypeID) -> void*
			{
				return AllocateData(aDataTypeID, aArena, aDefaultValue);
			},
			[this, &aArena](const StructID aStructID) -> void*
			{
				return AllocateStructInstance(aStructID, aArena);
			},
			[this, &aArena](const ClassID aClassID) -> void*
			{
				return AllocateClassInstance(aClassID, aArena);
			}
			}, aDataTypeID.mID
		);
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