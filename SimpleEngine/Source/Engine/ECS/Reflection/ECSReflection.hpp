#pragma once
#include "Engine/ECS/Core/Entity.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"
#include "Engine/ECS/Reflection/ECSEditorFunctions.hpp"
#include "External/nlohmann/json.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <concepts>
#include <typeindex>

inline static nlohmann::json TestSaveData(float& aValue, const std::string aVariableName)
{
	nlohmann::json json;
	json[aVariableName] = aValue;
	return json;
}

struct ComponentProperty final
{
	std::string name = "UnknownProperty";
	size_t id = 0;
	size_t byteOffset = 0;
	bool shouldExpose = true;
};

class TypeErasureComponent final
{
public:
	std::vector<ComponentProperty> myComponentProperties;
	std::string myComponentName;

	bool(*AddComponentFunctionPointer)(ECS::Entity aEntity) = nullptr;
	bool(*EditorFunctionPointer)(void* aData, const std::string& aVariableName) = nullptr;

	nlohmann::json(*TestSaveLoadData)(void* aData, const std::string aVariableName) = nullptr;
};

template<typename T>
concept Editable = requires(T & aData, const std::string & aVariableName)
{
	{ ViewAndEditValue(aData, aVariableName) } -> std::same_as<bool>;
};

template<typename T>
concept Editable2 = requires(T & aData, const std::string aVariableName)
{
	{ TestSaveData(aData, aVariableName) } -> std::same_as<nlohmann::json>;
};

class ComponentRegistry final
{
public:
	static inline std::unordered_map<size_t, TypeErasureComponent> myTypeErasureComponents = {};
	static inline std::unordered_map<size_t, TypeErasureComponent> myTypeErasureDataTypes = {};
	static inline std::unordered_map<std::type_index, void(*)(void*)> myTypeErasureComponentDestructorInvoker;
public:

	template<typename T>
	static inline void RegisterComponentType()
	{
		const bool alreadyExistOrHashCollision = myTypeErasureComponents.contains(typeid(T).hash_code());

		if (alreadyExistOrHashCollision == true)
		{
			assert(false && "Component already exist or has hash collision!");
			return;
		}

		TypeErasureComponent typeErasureComponent;

		typeErasureComponent.myComponentName = SimpleUtilities::ConvertTypeIndexNameToPrettyName(typeid(T).name());

		typeErasureComponent.AddComponentFunctionPointer = [](ECS::Entity aEntity) -> bool
			{
				return aEntity->AddComponent<T>();
			};

		if constexpr (Editable<T>)
		{
			typeErasureComponent.EditorFunctionPointer = [](void* aDataPointer, const std::string& aVariableName) -> bool
				{
					T* pointer = reinterpret_cast<T*>(aDataPointer);
					return ViewAndEditValue(*pointer, aVariableName + "##" + std::to_string(reinterpret_cast<size_t>(aDataPointer)));
				};
		}

		myTypeErasureComponents[typeid(T).hash_code()] = typeErasureComponent;

		myTypeErasureComponentDestructorInvoker[typeid(T)] = [](void* aPointer) -> void
			{
				static_cast<T*>(aPointer)->~T();
			};
	}

	template<typename T>
	static inline void RegisterDataType()
	{
		const bool alreadyExistOrHashCollision = myTypeErasureDataTypes.contains(typeid(T).hash_code());

		if (alreadyExistOrHashCollision == true)
		{
			assert(false && "Component already exist or has hash collision!");
			return;
		}

		TypeErasureComponent dataType;

		dataType.myComponentName = SimpleUtilities::ConvertTypeIndexNameToPrettyName(typeid(T).name());

		dataType.AddComponentFunctionPointer = [](ECS::Entity aEntity) -> bool
			{
				return aEntity->AddComponent<T>();
			};

		if constexpr (Editable<T>)
		{
			dataType.EditorFunctionPointer = [](void* aDataPointer, const std::string& aVariableName) -> bool
				{
					T* pointer = reinterpret_cast<T*>(aDataPointer);
					return ViewAndEditValue(*pointer, aVariableName + "##" + std::to_string(reinterpret_cast<size_t>(aDataPointer)));
				};
		}

		if constexpr (Editable2<T>)
		{
			dataType.TestSaveLoadData = [](void* aDataPointer, const std::string aVariableName) -> nlohmann::json
				{
					T* pointer = reinterpret_cast<T*>(aDataPointer);
					return TestSaveData(*pointer, aVariableName);
				};
		}

		myTypeErasureDataTypes[typeid(T).hash_code()] = dataType;
	}

	static inline void ExposeProperty(size_t aHashCode, void* aData, const std::string& aVariableName = "")
	{
		auto it = myTypeErasureDataTypes.find(aHashCode);

		if (it != myTypeErasureDataTypes.end())
		{
			if (it->second.EditorFunctionPointer != nullptr)
			{
				it->second.EditorFunctionPointer(aData, aVariableName);
			}

			for (auto& componentProperty : it->second.myComponentProperties)
			{
				ExposeProperty(componentProperty.id, reinterpret_cast<void*>((reinterpret_cast<size_t>(aData) + componentProperty.byteOffset)), componentProperty.name);
			}
		}
	}

	static inline void InspectComponentProperties(size_t aHashCode, void* aData, const std::string& aVariableName = "")
	{
		auto it = myTypeErasureComponents.find(aHashCode);

		if (it != myTypeErasureComponents.end())
		{
			if (it->second.EditorFunctionPointer != nullptr)
			{
				it->second.EditorFunctionPointer(aData, aVariableName);
			}

			for (auto& componentProperty : it->second.myComponentProperties)
			{
				if (componentProperty.shouldExpose == false)
				{
					continue;
				}

				ExposeProperty(componentProperty.id, reinterpret_cast<void*>((reinterpret_cast<size_t>(aData) + componentProperty.byteOffset)), componentProperty.name);
			}
		}
	}

	template<typename DataType, typename PropertyType>
	static inline constexpr size_t GetByteOffset(PropertyType DataType::* aProperty)
	{
		constexpr DataType* nullDataType = nullptr;
		return (size_t) & reinterpret_cast<const char&>(nullDataType->*aProperty); //NOTE(v11.0.1): this is supposed to be null?
	}

	template<typename DataType, typename Component>
	static inline void RegisterProperty(DataType Component::* aVariable, const std::string& aVariableName, const bool aShouldExpose)
	{
		ComponentProperty componentProperty;
		componentProperty.name = aVariableName;
		componentProperty.id = typeid(DataType).hash_code();
		componentProperty.byteOffset = GetByteOffset(aVariable);
		componentProperty.shouldExpose = aShouldExpose;

		const bool componentDoesExist = myTypeErasureComponents.contains(typeid(Component).hash_code());

		if (componentDoesExist == false)
		{
			assert(false && "Component type does not exist in register. Please register the component first before it's properties");
			return;
		}

		myTypeErasureComponents[typeid(Component).hash_code()].myComponentProperties.push_back(componentProperty);
	}
};

template<typename T>
struct __RegisterComponent final
{
	__RegisterComponent()
	{
		ComponentRegistry::RegisterComponentType<T>();
	}
};

template<typename T>
struct __RegisterDataType final
{
	__RegisterDataType()
	{
		ComponentRegistry::RegisterDataType<T>();
	}
};

struct __RegisterProperty final
{
	template<typename DataType, typename Component>
	__RegisterProperty(DataType Component::* aVariable, const char* aVariableName, const bool aShouldExpose = true)
	{
		ComponentRegistry::RegisterProperty(aVariable, aVariableName, aShouldExpose);
	}
};

#include "Engine/ECS/Reflection/ECSMacros.hpp"

#define REGISTER_DATATYPE(aDataType) inline __RegisterDataType<aDataType> registerType##aDataType;
#define REGISTER_COMPONENT(aComponent) inline __RegisterComponent<aComponent> registerType##aComponent;
#define REGISTER_AND_EXPOSE_PROPERTY(aVariable, ...) inline __RegisterProperty COMBINE_FOR_UNIQUE_NAME(registerType_, __COUNTER__) = __RegisterProperty(aVariable, ExtractVariableNameFromDataTypeName(CONVERT_TO_STRING(aVariable)), __VA_ARGS__); //NOTE(v11.0.0): where does __COUNTER__ macro came from?. But it works.

//TO-DO(v11.0.3): maybe figure out a more modular way to register different type of const, pointers and array of different sizes
#define REGISTER_DATATYPE_CONST_POINTER(aDataType) inline __RegisterDataType<const aDataType*> registerTypeConstPointer##aDataType;
#define REGISTER_DATATYPE_ARRAY_CONST_POINTER(aArray, aDataType, aSize) inline __RegisterDataType<aArray<const aDataType*, aSize>> registerTypeArrayConstPointer##aArray;