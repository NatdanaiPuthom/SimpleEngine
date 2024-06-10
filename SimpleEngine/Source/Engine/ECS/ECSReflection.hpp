#pragma once
#include "Engine/ECS/Core/Entity.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"
#include "Engine/ECS/ECSEditorFunctions.hpp"
#include <string>
#include <unordered_map>
#include <concepts>

bool EditValue(bool& aValue, const std::string& aVariableName);
bool EditValue(int& aValue, const std::string& aVariableName);
bool EditValue(float& aValue, const std::string& aVariableName);
bool EditValue(char& aValue, const std::string& aVariableName);

bool EditValue(const std::string& aValue, const std::string& aVariableName);

bool EditValue(Math::Vector2f& aValue, const std::string& aVariableName);
bool EditValue(Math::Vector3f& aValue, const std::string& aVariableName);
bool EditValue(Math::Vector4f& aValue, const std::string& aVariableName);
bool EditValue(Math::Transform& aValue, const std::string& aVariableName);

struct ComponentProperty
{
	std::string name;
	size_t id = 0;
	size_t byteOffset = 0;
};

class TypeErasureComponent final
{
public:
	std::vector<ComponentProperty> myComponentProperties;
	std::string myComponentName;

	bool(*AddComponentFunctionPointer)(ECS::Entity aEntity) = nullptr;
	bool(*EditorFunctionPointer)(void* aData, const std::string& aVariableName) = nullptr;
};

template<typename T>
concept Editable = requires(T & aData, const std::string & aVariableName)
{
	{ EditValue(aData, aVariableName) } -> std::same_as<bool>;
};

class ComponentRegistry
{
public:
	static inline std::unordered_map<size_t, TypeErasureComponent> myTypeErasureComponents = {};
	static inline std::unordered_map<size_t, TypeErasureComponent> myTypeErasureDataTypes = {};
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
					return EditValue(*pointer, aVariableName + "##" + std::to_string(reinterpret_cast<size_t>(aDataPointer)));
				};
		}

		myTypeErasureComponents[typeid(T).hash_code()] = typeErasureComponent;
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
					return EditValue(*pointer, aVariableName + "##" + std::to_string(reinterpret_cast<size_t>(aDataPointer)));
				};
		}

		myTypeErasureDataTypes[typeid(T).hash_code()] = typeErasureComponent;
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
	static inline void RegisterProperty(DataType Component::* aVariable, const std::string& aVariableName)
	{
		ComponentProperty componentProperty;
		componentProperty.name = aVariableName;
		componentProperty.id = typeid(DataType).hash_code();
		componentProperty.byteOffset = GetByteOffset(aVariable);

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
	__RegisterProperty(DataType Component::* aVariable, const char* aVariableName)
	{
		ComponentRegistry::RegisterProperty(aVariable, aVariableName);
	}
};

#define DATATYPE_NAME(aName) #aName
#define CONVERT_TO_STRING(aName) DATATYPE_NAME(aName)

#define PROPERTY_DETAIL(aName, aNumberCounter) aName##aNumberCounter
#define COMBINE_FOR_UNIQUE_NAME(aName, aNumberCounter) PROPERTY_DETAIL(aName, aNumberCounter)

#define REGISTER_DATA_TYPE(aDataType) inline __RegisterDataType<aDataType> registerType##aDataType;
#define REGISTER_COMPONENT(aComponent) inline __RegisterComponent<aComponent> registerType##aComponent;
#define REGISTER_PROPERTY(aVariable) inline __RegisterProperty COMBINE_FOR_UNIQUE_NAME(registerType_, __COUNTER__) = __RegisterProperty(aVariable, ExtractVariableNameFromDataTypeName(CONVERT_TO_STRING(aVariable))); //NOTE(v11.0.0): where does __COUNTER__ macro came from?. But it works.