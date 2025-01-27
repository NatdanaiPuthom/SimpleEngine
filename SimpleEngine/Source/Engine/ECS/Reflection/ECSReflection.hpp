#pragma once
#include "Engine/SimpleUtilities/Utility.hpp"
#include "Engine/ECS/Core/Entity.hpp"
#include "Engine/ECS/Reflection/ECSEditorFunctions.hpp"
#include "Engine/ECS/Reflection/ECSSaveData.hpp"
#include "Engine/ECS/Reflection/ECSLoadData.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <concepts>
#include <typeindex>

class MainSingleton;

namespace ECS
{
	using InPlaceAllocateFunction = void(*)(void* aData, const void* aDefaultValuePtr);
	using DestroyFunction = void(*)(void* aData);
	using CopyFunction = void (*)(void* aDestination, const void* aSource);
	using SwapFunction = void (*)(void* aDataPtr1, void* aDataPtr2);
	using ComponentHashCode = size_t;
}

namespace ECS
{
	class __RegisterProperty;
	template<typename T> class __RegisterComponent;
	template<typename T> class __RegisterDataType;
}

namespace ECS
{
	template<typename T>
	concept Editable = requires(T & aData, const std::string & aVariableName)
	{
		{ ECS::ViewAndEditValue(aData, aVariableName) } -> std::same_as<ViewAndEditResult>;
	};

	template<typename T>
	concept Saveable = requires(T & aData, const std::string & aVariableName)
	{
		{ ECS::ReturnDataAsJSON(aData, aVariableName) } -> std::same_as<nlohmann::json>;
	};

	template<typename T>
	concept Loadable = requires(T & aData, const std::string & aVariableName, const nlohmann::json & aJSONData)
	{
		{ ECS::LoadAndSetDataFromJSON(aData, aVariableName, aJSONData) } -> std::same_as<bool>;
	};

	struct ComponentProperty final
	{
		std::string name = "UnknownProperty";
		std::string customVariableName = "UnknownCustomNameProperty";
		size_t id = 0;
		size_t byteOffset = 0;
		bool shouldExpose = true;
		bool canEdit = true;
	};

	class TypeErasureObject final
	{
	public:
		std::string myComponentName;
		std::string myComponentPrettyName;
		std::vector<ComponentProperty> myComponentProperties;

		ComponentID(*AddComponentFunctionPointer)(ECS::Entity& aEntity) = nullptr;
		ViewAndEditResult(*EditorFunctionPointer)(void* aData, const std::string& aVariableName) = nullptr;

		nlohmann::json(*GetDataAsJSON)(void* aData, const std::string& aVariableName) = nullptr;
		bool (*LoadDataFromJSON)(void* aData, const std::string& aVariableName, const nlohmann::json& aJSONData) = nullptr;

		InPlaceAllocateFunction InplaceAllocate = nullptr;
		DestroyFunction Destroy = nullptr;
		DestroyFunction DeleteComponentVector = nullptr;
		CopyFunction CopyFunctionPointer = nullptr;
		SwapFunction SwapFunctionPointer = nullptr;

		bool myIsComponent = false;

		size_t mySize = 0;
		bool hasBeenAdded = false;
	private:
		char myPadding[7] = "Belie\0";
	};

	class ComponentRegistry final
	{
		using ComponentName = std::string;

		friend class MainSingleton;
		friend class __RegisterProperty;
		template<typename T> friend class __RegisterComponent;
		template<typename T> friend class __RegisterDataType;

	public:
		std::unordered_map<ComponentHashCode, TypeErasureObject> myTypeErasureComponents;
		std::unordered_map<ComponentHashCode, TypeErasureObject> myTypeErasureDataTypes;
		std::unordered_map<ComponentName, ComponentHashCode> myComponentNameToHashCode;
	public:

		ViewAndEditResult InspectComponentProperties(size_t aHashCode, void* aData, const std::string& aVariableName = "") const;

		ComponentID AddComponent(ComponentHashCode aHashCode, Entity& aEntity) const;

		void InplaceAllocateComponent(ComponentHashCode aHashCode, void* aDestination, const void* aDefaultValuePtr = nullptr) const;
		void CopyComponent(ComponentHashCode aHashCode, void* aDestination, const void* aSource) const;
		void SwapComponent(ComponentHashCode aHashCode, void* aDataPtr1, void* aDataPtr2) const;
		void DestroyComponent(ComponentHashCode aHashCode, void* aDataPtr) const;
		void DeleteComponentVector(ComponentHashCode aHashCode, void* aDataPtr) const;

	public:
		size_t GetComponentSize(ComponentHashCode aHashCode) const;
		InPlaceAllocateFunction GetInplaceAllocateFunction(ComponentHashCode aHashCode) const;
		DestroyFunction GetDestroyFunction(ComponentHashCode aHashCode) const;
		CopyFunction GetCopyFunction(ComponentHashCode aHashCode) const;
	private:
		//NOTE(v11.1.2): Calling GetInstance first time is not thread safe but shouldn't matter since there should never be multiple threads calling GetInstance at the sametime during dynamic initialization phase...?
		static ComponentRegistry* GetInstance();

		void Destroy();

		ViewAndEditResult ExposeProperty(size_t aHashCode, void* aData, const std::string& aVariableName = "") const;

		template<typename T>
		void RegisterComponentType();

		template<typename T>
		void RegisterDataType();

		template<typename DataType, typename Component>
		void RegisterProperty(DataType Component::* aVariable, const std::string& aVariableName, const char* aCustomName, const bool aShouldExpose, const bool aCanEdit);

		template<typename DataType, typename PropertyType>
		constexpr size_t GetByteOffset(PropertyType DataType::* aProperty);
	private:
		ComponentRegistry();
		~ComponentRegistry();

		ComponentRegistry(const ComponentRegistry&) = delete;
		ComponentRegistry(const ComponentRegistry&&) = delete;
		ComponentRegistry operator=(const ComponentRegistry&) = delete;
		ComponentRegistry operator=(const ComponentRegistry&&) = delete;
	private:
		inline static ComponentRegistry* myPtr = nullptr;
	};

	template<typename T>
	inline void ComponentRegistry::RegisterComponentType()
	{
		const ComponentHashCode hashCode = typeid(T).hash_code();
		const bool alreadyExistOrHashCollision = myTypeErasureComponents.contains(hashCode);

		if (alreadyExistOrHashCollision == true && myTypeErasureComponents[hashCode].hasBeenAdded)
		{
			return;
		}

		TypeErasureObject typeErasureComponent;
		typeErasureComponent.hasBeenAdded = true;
		typeErasureComponent.mySize = sizeof(T);

		typeErasureComponent.myComponentName = SimpleUtilities::ConvertTypeIndexNameToPrettyName(typeid(T).name());
		typeErasureComponent.myComponentPrettyName = SimpleUtilities::RemoveSubStringIfExist(typeErasureComponent.myComponentName, "Component");

		typeErasureComponent.AddComponentFunctionPointer = [](ECS::Entity& aEntity) -> ComponentID
			{
				return aEntity.AddComponent<T>();
			};

		typeErasureComponent.InplaceAllocate = [](void* aDataPtr, const void* aDefaultValuePtr) -> void
			{
				if (aDefaultValuePtr != nullptr)
				{
					const T& defaultValue = *reinterpret_cast<const T*>(aDefaultValuePtr);
					new(aDataPtr)T(defaultValue);
				}
				else
				{
					new(aDataPtr)T();
				}
			};

		typeErasureComponent.Destroy = [](void* aDataPtr) -> void
			{
				T& value = *reinterpret_cast<T*>(aDataPtr);
				value.~T();
			};

		typeErasureComponent.DeleteComponentVector = [](void* aDataPtr) -> void
			{
				std::vector<T*>* originalVector = reinterpret_cast<std::vector<T*>*>(aDataPtr);
				delete originalVector;
			};

		typeErasureComponent.CopyFunctionPointer = [](void* aDestination, const void* aSource) -> void
			{
				if constexpr (std::is_trivially_copyable_v<T>)
				{
					memcpy(aDestination, aSource, sizeof(T));
				}
				else
				{
					T& destination = *reinterpret_cast<T*>(aDestination);
					const T& source = *reinterpret_cast<const T*>(aSource);
					destination = source;
				}
			};

		typeErasureComponent.SwapFunctionPointer = [](void* aDataPtr1, void* aDataPtr2) -> void
			{
				using std::swap;

				T& value1 = *reinterpret_cast<T*>(aDataPtr1);
				T& value2 = *reinterpret_cast<T*>(aDataPtr2);
				swap(value1, value2);
			};


		if constexpr (ECS::Editable<T>)
		{
			typeErasureComponent.EditorFunctionPointer = [](void* aDataPointer, const std::string& aVariableName) -> bool
				{
					T* pointer = reinterpret_cast<T*>(aDataPointer);
					return ECS::ViewAndEditValue(*pointer, aVariableName + "##" + std::to_string(reinterpret_cast<size_t>(aDataPointer)));
				};
		}

		myTypeErasureComponents[hashCode] = typeErasureComponent;

		myComponentNameToHashCode[typeErasureComponent.myComponentName] = hashCode;
	}

	template<typename T>
	inline void ComponentRegistry::RegisterDataType()
	{
		const bool alreadyExistOrHashCollision = myTypeErasureDataTypes.contains(typeid(T).hash_code());

		if (alreadyExistOrHashCollision == true)
		{
			assert(false && "Component already exist or has hash collision!");
			return;
		}

		TypeErasureObject dataType;

		dataType.myComponentName = SimpleUtilities::ConvertTypeIndexNameToPrettyName(typeid(T).name());

		dataType.AddComponentFunctionPointer = [](ECS::Entity& aEntity) -> ComponentID
			{
				return aEntity.AddComponent<T>();
			};

		if constexpr (ECS::Editable<T>)
		{
			dataType.EditorFunctionPointer = [](void* aDataPointer, const std::string& aVariableName) -> ViewAndEditResult
				{
					T* pointer = reinterpret_cast<T*>(aDataPointer);
					return ECS::ViewAndEditValue(*pointer, aVariableName + "##" + std::to_string(reinterpret_cast<size_t>(aDataPointer)));
				};
		}

		if constexpr (ECS::Saveable<T>)
		{
			dataType.GetDataAsJSON = [](void* aDataPointer, const std::string& aVariableName) -> nlohmann::json
				{
					T* pointer = reinterpret_cast<T*>(aDataPointer);
					return ECS::ReturnDataAsJSON(*pointer, aVariableName);
				};
		}

		if constexpr (ECS::Loadable<T>)
		{
			dataType.LoadDataFromJSON = [](void* aDataPointer, const std::string& aVariableName, const nlohmann::json& aJSONData) -> bool
				{
					T* pointer = reinterpret_cast<T*>(aDataPointer);
					return ECS::LoadAndSetDataFromJSON(*pointer, aVariableName, aJSONData);
				};
		}

		myTypeErasureDataTypes[typeid(T).hash_code()] = dataType;
	}

	template<typename DataType, typename Component>
	inline void ComponentRegistry::RegisterProperty(DataType Component::* aVariable, const std::string& aVariableName, const char* aCustomName, const bool aShouldExpose, const bool aCanEdit)
	{
		ComponentProperty componentProperty;
		componentProperty.name = aVariableName;
		componentProperty.id = typeid(DataType).hash_code();
		componentProperty.byteOffset = GetByteOffset(aVariable);
		componentProperty.shouldExpose = aShouldExpose;
		componentProperty.canEdit = aCanEdit;

		if (aCustomName != nullptr)
		{
			componentProperty.customVariableName = aCustomName;
		}
		else
		{
			componentProperty.customVariableName = SimpleUtilities::ConvertAndAddSpaceToSubStringWithUpperCase(aVariableName);
		}

		const bool componentDoesExist = myTypeErasureComponents.contains(typeid(Component).hash_code());

		if (componentDoesExist == false)
		{
			RegisterComponentType<Component>(); //NOTE(v11.3.4): In case the inline order happen to choose REGISTER_PROPERTY first :(
		}

		myTypeErasureComponents[typeid(Component).hash_code()].myComponentProperties.push_back(componentProperty);
	}

	template<typename DataType, typename PropertyType>
	inline constexpr size_t ComponentRegistry::GetByteOffset(PropertyType DataType::* aProperty)
	{
		constexpr DataType* nullDataType = nullptr;
		return (size_t) & reinterpret_cast<const char&>(nullDataType->*aProperty); //NOTE(v11.1.2): Why does this work? How?
	}

	template<typename T>
	class __RegisterComponent final
	{
	public:
		__RegisterComponent()
		{
			ECS::ComponentRegistry::GetInstance()->RegisterComponentType<T>();
		}
	};

	template<typename T>
	class __RegisterDataType final
	{
	public:
		__RegisterDataType()
		{
			ECS::ComponentRegistry::GetInstance()->RegisterDataType<T>();
		}
	};

	class __RegisterProperty final
	{
	public:
		template<typename DataType, typename Component>
		__RegisterProperty(DataType Component::* aVariable, const char* aVariableName, const char* aCustomName = nullptr, const bool aShouldExpose = true, const bool aCanEdit = true)
		{
			ECS::ComponentRegistry::GetInstance()->RegisterProperty(aVariable, aVariableName, aCustomName, aShouldExpose, aCanEdit);
		}
	};
}

#include "Engine/SimpleUtilities/MacroUtility.hpp"

//NOTE(v12.0.0): Anything below this is generated by ChatGPT. It works with trial and errors but I have no clue what's going on, please kindly, don't ask and just fix it if there's error :)

#define COMPONENT(ComponentType) \
    struct ComponentType; \
    inline static bool ComponentRegistered_##ComponentType = []() { \
        ECS::__RegisterComponent<ComponentType> Reflection_ECS_Registered_Component_##ComponentType; \
        return true; \
    }(); \
    struct ComponentType

#define REGISTER_DATATYPE(aDataType) inline ECS::__RegisterDataType<aDataType> Reflection_ECS_Registered_Datatype_##aDataType;

#define REGISTER_AND_EXPOSE_PROPERTY(aVariable, ...) \
	inline ECS::__RegisterProperty COMBINE_FOR_UNIQUE_NAME(Reflection_ECS_Registered_Property_, __COUNTER__) = \
		ECS::__RegisterProperty(aVariable, ECS::ExtractVariableNameFromDataTypeName(CONVERT_TO_STRING(aVariable)), __VA_ARGS__); //NOTE(v11.0.0): where does __COUNTER__ macro came from?. But it works.

//TO-DO(v11.0.3): maybe figure out a more modular way to register different type of const, pointers and array of different sizes
#define REGISTER_DATATYPE_CONST_POINTER(aDataType) inline ECS::__RegisterDataType<const aDataType*> Reflection_ECS_Registered_Datatype_Const_Pointer_##aDataType;
#define REGISTER_DATATYPE_POINTER(aDataType) inline ECS::__RegisterDataType<aDataType*> Reflection_ECS_Registered_Datatype_Pointer_##aDataType;
#define REGISTER_DATATYPE_ARRAY_CONST_POINTER(aArray, aDataType, aSize) inline ECS::__RegisterDataType<aArray<const aDataType*, aSize>> Reflection_ECS_Registered_Datatype_Array_Const_Pointer_##aArray;