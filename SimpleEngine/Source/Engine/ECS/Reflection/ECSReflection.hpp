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
	class __RegisterProperty;
	template<typename T> class __RegisterComponent;
	template<typename T> class __RegisterDataType;
}

namespace ECS
{
	template<typename T>
	concept Editable = requires(T & aData, const std::string & aVariableName)
	{
		{ ECS::ViewAndEditValue(aData, aVariableName) } -> std::same_as<bool>;
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

		const ComponentID(*AddComponentFunctionPointer)(ECS::Entity& aEntity) = nullptr;
		bool(*EditorFunctionPointer)(void* aData, const std::string& aVariableName) = nullptr;

		nlohmann::json(*GetDataAsJSON)(void* aData, const std::string& aVariableName) = nullptr;
		bool (*LoadDataFromJSON)(void* aData, const std::string& aVariableName, const nlohmann::json& aJSONData) = nullptr;

		void (*CreateComponent)(void* aDestination, const void* aSource) = nullptr;
		void (*CopyFunctionPointer)(void* aDestination, const void* aSource) = nullptr;

		bool hasBeenAdded = false;
	private:
		char myPadding[8] = "Believ\0";
	};

	class ComponentRegistry final
	{
		using ComponentHashCode = size_t;
		using ComponentName = std::string;

		friend class MainSingleton;
		friend class __RegisterProperty;
		template<typename T> friend class __RegisterComponent;
		template<typename T> friend class __RegisterDataType;

	public:
		std::unordered_map<ComponentHashCode, TypeErasureObject> myTypeErasureComponents;
		std::unordered_map<ComponentHashCode, TypeErasureObject> myTypeErasureDataTypes;
		std::unordered_map<ComponentName, ComponentHashCode> myComponentNameToHashCode;
		std::unordered_map<ComponentHashCode, void(*)(void*)> myTypeErasureComponentDestructorInvoker;
	public:
		void InspectComponentProperties(size_t aHashCode, void* aData, const std::string& aVariableName = "");

		ComponentRegistry(const ComponentRegistry&) = delete;
		ComponentRegistry(const ComponentRegistry&&) = delete;
		ComponentRegistry operator=(const ComponentRegistry&) = delete;
		ComponentRegistry operator=(const ComponentRegistry&&) = delete;

	private:
		//NOTE(v11.1.2): Calling GetInstance first time is not thread safe but shouldn't matter since there should never be multiple threads calling GetInstance at the sametime during dynamic initialization phase...?
		static ComponentRegistry* GetInstance();

		void Destroy();

		void ExposeProperty(size_t aHashCode, void* aData, const std::string& aVariableName = "");

		template<typename T>
		void RegisterComponentType();

		template<typename T>
		void RegisterDataType();

		template<typename DataType, typename Component>
		void RegisterProperty(DataType Component::* aVariable, const std::string& aVariableName, const bool aShouldExpose, const bool aCanEdit);

		template<typename DataType, typename PropertyType>
		constexpr size_t GetByteOffset(PropertyType DataType::* aProperty);
	private:
		ComponentRegistry();
		~ComponentRegistry();
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

		typeErasureComponent.myComponentName = SimpleUtilities::ConvertTypeIndexNameToPrettyName(typeid(T).name());
		typeErasureComponent.myComponentPrettyName = SimpleUtilities::RemoveSubStringIfExist(typeErasureComponent.myComponentName, "Component");

		typeErasureComponent.AddComponentFunctionPointer = [](ECS::Entity& aEntity) -> const ComponentID
			{
				return aEntity.AddComponent<T>();
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

		typeErasureComponent.CreateComponent = [](void* aDestination, const void* aSource) -> void
			{
				const T& source = *reinterpret_cast<const T*>(aSource);
				new(aDestination)T(source);
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

		myTypeErasureComponentDestructorInvoker[typeid(T).hash_code()] = [](void* aPointer) -> void
			{
				static_cast<T*>(aPointer)->~T();
			};

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

		dataType.AddComponentFunctionPointer = [](ECS::Entity& aEntity) -> const ComponentID
			{
				return aEntity.AddComponent<T>();
			};

		if constexpr (ECS::Editable<T>)
		{
			dataType.EditorFunctionPointer = [](void* aDataPointer, const std::string& aVariableName) -> bool
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
	inline void ComponentRegistry::RegisterProperty(DataType Component::* aVariable, const std::string& aVariableName, const bool aShouldExpose, const bool aCanEdit)
	{
		ComponentProperty componentProperty;
		componentProperty.name = aVariableName;
		componentProperty.id = typeid(DataType).hash_code();
		componentProperty.byteOffset = GetByteOffset(aVariable);
		componentProperty.shouldExpose = aShouldExpose;
		componentProperty.canEdit = aCanEdit;

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
		__RegisterProperty(DataType Component::* aVariable, const char* aVariableName, const bool aShouldExpose = true, const bool aCanEdit = true)
		{
			ECS::ComponentRegistry::GetInstance()->RegisterProperty(aVariable, aVariableName, aShouldExpose, aCanEdit);
		}
	};
}

#include "Engine/SimpleUtilities/MacroUtility.hpp"

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