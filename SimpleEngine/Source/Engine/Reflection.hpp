#pragma once
#include "Engine/ECS/Core/Entity.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"
#include "External/imgui.h"
#include <string>
#include <unordered_map>
#include <concepts>

inline bool EditValue(int& aValue, const std::string& aVariableName)
{
	return ImGui::DragInt(aVariableName.c_str(), &aValue);
}

inline bool EditValue(bool& aValue, const std::string& aVariableName)
{
	return ImGui::Checkbox(aVariableName.c_str(), &aValue);
}

struct ComponentProperty
{
	std::string name;
	size_t id;
	size_t byteOffset;
};

struct TypeErasureComponent
{
	std::vector<ComponentProperty> componentProperties;
	std::string componentName;

	bool(*AddComponentFunctionPointer)(ECS::Entity aEntity) = nullptr;
	bool(*Edit)(void* aData, const std::string& aVariableName) = nullptr;
};

template<typename T>
concept Editable = requires(T & aData, const std::string& aVariableName)
{
	{ EditValue(aData, aVariableName) } -> std::same_as<bool>;
};

class Component_Registry
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
		typeErasureComponent.componentName = SimpleUtilities::ConvertTypeIndexNameToPrettyName(typeid(T).name());

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

			for (auto& test : it->second.componentProperties)
			{
				Edit(test.id, reinterpret_cast<void*>((reinterpret_cast<size_t>(aData) + test.byteOffset)), test.name);
			}
		}
	}

	template<typename ClassType, typename PropertyType>
	static inline constexpr size_t GetByteOffset(PropertyType ClassType::* aProperty)
	{
		constexpr ClassType* a = nullptr;
		return (size_t) & reinterpret_cast<const char&>(a->*aProperty);
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

		myTypeErasureComponents[typeid(Component).hash_code()].componentProperties.push_back(componentProperty);
	}
};