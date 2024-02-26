#pragma once
#include <typeindex>
#include <vector>
#include <cstring>
#include <unordered_map>

using ComponentCount = std::size_t;
using Components = std::vector<char>;
using ComponentID = std::type_index;

class ComponentManager
{
public:
	ComponentManager();
	~ComponentManager();

	template<typename ComponentType>
	inline void AddComponent(const ComponentType& aComponent);

	template<typename ComponentType>
	inline ComponentType& GetComponent(const int aIndex);

	template<typename ComponentType>
	inline std::vector<ComponentType>& GetAllComponentsOfType();
private:
	std::unordered_map<ComponentID, std::pair<ComponentCount, Components>> myComponents;
};

template<typename ComponentType>
inline void ComponentManager::AddComponent(const ComponentType& aComponent)
{
	auto it = myComponents.find(typeid(aComponent));

	if (it != myComponents.end())
	{
		ComponentCount& count = myComponents[typeid(aComponent)].first;
		Components& component = myComponents[typeid(aComponent)].second;

		if (component.size() <= count * sizeof(aComponent))
		{
			component.resize((count * 2) * sizeof(ComponentType));
		}

		std::memcpy(&component[count * sizeof(ComponentType)], &aComponent, sizeof(ComponentType));
		++count;
	}
	else
	{
		ComponentCount& count = myComponents[typeid(aComponent)].first;
		Components& component = myComponents[typeid(aComponent)].second;

		if (component.size() <= count * sizeof(aComponent))
		{
			component.resize((count + 1) * sizeof(ComponentType));
		}

		std::memcpy(&component[count * sizeof(ComponentType)], &aComponent, sizeof(ComponentType));
		++count;
	}
}

template<typename ComponentType>
inline ComponentType& ComponentManager::GetComponent(const int aIndex)
{
	char* componentPointer = &myComponents[typeid(ComponentType)].second[aIndex * sizeof(ComponentType)];
	return *reinterpret_cast<ComponentType*>(componentPointer);
}

template<typename ComponentType>
inline std::vector<ComponentType>& ComponentManager::GetAllComponentsOfType()
{
	Components& componentBytes = myComponents[typeid(ComponentType)].second;

	const ComponentCount componentCount = myComponents[typeid(ComponentType)].first;
	const std::size_t elementCount = componentBytes.size() / sizeof(ComponentType);

	if (elementCount != componentCount) //This important
	{
		componentBytes.resize(componentCount * sizeof(ComponentType));
	}

	return reinterpret_cast<std::vector<ComponentType>&>(componentBytes);
}