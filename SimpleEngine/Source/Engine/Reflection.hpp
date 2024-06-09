#pragma once
#include "Engine/ECS/Core/Entity.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"
#include <string>
#include <unordered_map>
#include <concepts>

bool EditValue(int& aValue, const std::string& aVariableName);
bool EditValue(bool& aValue, const std::string& aVariableName);

enum class eTypeTraits
{
	Primitive,
	StructOrClass,
	NotAssigned
};

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

	eTypeTraits myTypeTrait;

	bool(*AddComponentFunctionPointer)(ECS::Entity aEntity) = nullptr;
	bool(*Edit)(void* aData, const std::string& aVariableName) = nullptr;
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
		typeErasureComponent.myTypeTrait = eTypeTraits::NotAssigned;

		if (std::is_fundamental_v<T>)
		{
			typeErasureComponent.myTypeTrait = eTypeTraits::Primitive;
		}
		else
		{
			typeErasureComponent.myTypeTrait = eTypeTraits::StructOrClass;
		}

		typeErasureComponent.myComponentName = SimpleUtilities::ConvertTypeIndexNameToPrettyName(typeid(T).name());

		typeErasureComponent.AddComponentFunctionPointer = [](ECS::Entity aEntity) -> bool
			{
				return aEntity->AddComponent<T>();
			};

		if constexpr (Editable<T>)
		{
			typeErasureComponent.Edit = [](void* aDataPointer, const std::string& aVariableName) -> bool
				{
					T* pointer = reinterpret_cast<T*>(aDataPointer);
					return EditValue(*pointer, aVariableName + "##" + std::to_string(reinterpret_cast<size_t>(aDataPointer)));
				};
		}

		myTypeErasureComponents[typeid(T).hash_code()] = typeErasureComponent;
	}

	static inline void Edit(size_t aHashCode, void* aData, const std::string& aVariableName = "")
	{
		auto it = myTypeErasureComponents.find(aHashCode);

		if (it != myTypeErasureComponents.end())
		{
			if (it->second.Edit != nullptr)
			{
				it->second.Edit(aData, aVariableName);
			}

			for (auto& componentProperty : it->second.myComponentProperties)
			{
				Edit(componentProperty.id, reinterpret_cast<void*>((reinterpret_cast<size_t>(aData) + componentProperty.byteOffset)), componentProperty.name);
			}
		}
	}

	template<typename ClassType, typename PropertyType>
	static inline constexpr size_t GetByteOffset(PropertyType ClassType::* aProperty)
	{
		constexpr ClassType* nullClassType = nullptr;
		return (size_t) & reinterpret_cast<const char&>(nullClassType->*aProperty);
	}

	template<typename DataType, typename Component>
	static inline void RegisterProperty(DataType Component::* aVariable, const std::string& aVariableName)
	{
		ComponentProperty componentProperty;
		componentProperty.name = aVariableName;
		componentProperty.id = typeid(DataType).hash_code();
		componentProperty.byteOffset = GetByteOffset(aVariable);

		const bool exist = myTypeErasureComponents.contains(typeid(Component).hash_code());
		if (exist == false)
		{
			assert(false && "Component type does not exist in register. Please register it first!");
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

struct __RegisterProperty final
{
	template<typename DataType, typename Component>
	__RegisterProperty(DataType Component::* aVariable, const char* aVariableName)
	{
		ComponentRegistry::RegisterProperty(aVariable, aVariableName);
	}
};

#define STRINGIFY(aName) #aName
#define TOSTRING(aName) STRINGIFY(aName)

#define CONCATENATE_DETAIL(x, y) x##y
#define CONCATENATE(x, y) CONCATENATE_DETAIL(x, y)

template <size_t N>
constexpr const char* StripScope(const char (&name)[N]) 
{
    for (size_t i = N - 1; i > 0; --i) 
	{
        if (name[i - 1] == ':') 
		{
            return name + i;
        }
    }

    return name;
}

//NOTE(v11.0.0): where does __COUNTER__ macro came from?. But it works.
#define REGISTER_PROPERTY(aVariable) \
 inline __RegisterProperty CONCATENATE(registerType_, __COUNTER__) = __RegisterProperty(aVariable, StripScope(TOSTRING(aVariable)));

#define REGISTER_COMPONENT(aComponent) inline __RegisterComponent<aComponent> registerType##aComponent;