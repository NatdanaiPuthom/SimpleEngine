#pragma once
#include <typeindex>
#include <vector>
#include <cstring>
#include <unordered_map>

class ComponentManager;

class Entity
{
	friend class ComponentManager;
public:

	template<typename ComponentType>
	inline ComponentType* GetComponent()
	{
		auto it = myComponents.find(typeid(ComponentType));

		if (it != myComponents.end()) 
		{
			return reinterpret_cast<ComponentType*>(it->second);
		}
		else 
		{
			return nullptr;
		}
	}
private:
	std::unordered_map<std::type_index, void*> myComponents;
};

class ComponentManager
{
	using ComponentCount = std::size_t;
	using Components = std::vector<char>;
	using ComponentID = std::type_index;
public:
	ComponentManager();
	~ComponentManager();

	template<typename ComponentType>
	inline void RemoveComponent(const int aIndex)
	{
		ComponentCount& count = myComponents[typeid(ComponentType)].first;
		Components& components = myComponents[typeid(ComponentType)].second;

		ComponentType* componentPointer = reinterpret_cast<ComponentType*>(&components[aIndex * sizeof(ComponentType)]);
		componentPointer->~ComponentType();

		--count;
	}

	template<typename ComponentType>
	inline void AddComponent(const ComponentType& aComponent, Entity& aEntity);

	template<typename ComponentType>
	inline ComponentType* GetComponent(const int aIndex);

	template<typename ComponentType>
	inline std::vector<ComponentType>& GetAllComponentsOfType();
private:
	std::unordered_map<ComponentID, std::pair<ComponentCount, Components>> myComponents;
};

template<typename ComponentType>
inline void ComponentManager::AddComponent(const ComponentType& aComponent, Entity& aEntity)
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

		char* componentPointer = &myComponents[typeid(ComponentType)].second[(count - 1) * sizeof(ComponentType)];
		aEntity.myComponents[typeid(aComponent)] = reinterpret_cast<ComponentType*>(componentPointer);
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

		char* componentPointer = &myComponents[typeid(ComponentType)].second[(count -1 ) * sizeof(ComponentType)];
		aEntity.myComponents[typeid(aComponent)] = reinterpret_cast<ComponentType*>(componentPointer);
	}
}

template<typename ComponentType>
inline ComponentType* ComponentManager::GetComponent(const int aIndex)
{
	char* componentPointer = &myComponents[typeid(ComponentType)].second[aIndex * sizeof(ComponentType)];
	return reinterpret_cast<ComponentType*>(componentPointer);
}

template<typename ComponentType>
inline std::vector<ComponentType>& ComponentManager::GetAllComponentsOfType()
{
	Components& componentBytes = myComponents[typeid(ComponentType)].second;

	const ComponentCount componentCount = myComponents[typeid(ComponentType)].first;
	const std::size_t elementCount = componentBytes.size() / sizeof(ComponentType);

	if (elementCount != componentCount)
	{
		componentBytes.resize(componentCount * sizeof(ComponentType));
	}

	return reinterpret_cast<std::vector<ComponentType>&>(componentBytes);
}