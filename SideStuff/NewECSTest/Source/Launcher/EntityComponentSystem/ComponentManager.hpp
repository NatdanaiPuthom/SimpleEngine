#pragma once
#include "ComponentStorage.hpp"
#include "EntityID.hpp"

#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <memory>

namespace Simple
{
	struct ComponentType final
	{
		std::type_index type;

		explicit ComponentType(std::type_index aType) : type(aType) {}

		operator std::type_index() const { return type; }

		bool operator==(const ComponentType& aOther) const { return type == aOther.type; }
		bool operator!=(const ComponentType& aOther) const { return type != aOther.type; }

		struct Hash
		{
			size_t operator()(const ComponentType& aComponentType) const { return std::hash<std::type_index>{}(aComponentType.type); }
		};
	};

	class ComponentManager
	{
	public:
		template<typename T>
		T* AddComponent(const EntityID aEntityID, const T& aComponent);

		template<typename T>
		bool RemoveComponent(const EntityID aEntityID);

		template<typename T>
		T* GetComponent(const EntityID aEntityID);

		bool RemoveAllComponents(const EntityID& aEntityID);	
		bool EntityHasComponent(const EntityID aEntityID, const ComponentType& aComponentType);

	private:
		template<typename T>
		ComponentStorage<T>& GetComponentStorage();

	private:
		std::unordered_map<ComponentType, std::shared_ptr<void>, ComponentType::Hash> myComponentStorages;
		std::unordered_map<ComponentType, std::unordered_set<EntityID, EntityID::Hash>, ComponentType::Hash> myComponentTypeToEntityIDs;
		std::unordered_map<EntityID, std::unordered_map<ComponentType, ComponentIndex, ComponentType::Hash>, EntityID::Hash> myEntityToComponentIndex;
		std::unordered_map<EntityID, std::unordered_set<ComponentType, ComponentType::Hash>, EntityID::Hash> myEntityToComponentTypes;
	};

	template<typename T>
	inline T* ComponentManager::AddComponent(const EntityID aEntityID, const T& aComponent)
	{
		ComponentStorage<T>& componentStorage = GetComponentStorage<T>();
		const ComponentIndex index = componentStorage.Create(aComponent);

		myEntityToComponentIndex[aEntityID][ComponentType(typeid(T))] = index;
		myEntityToComponentTypes[aEntityID].insert(ComponentType(typeid(T)));
		myComponentTypeToEntityIDs[ComponentType(typeid(T))].insert(aEntityID);

		return componentStorage.GetComponentAtIndex(index);
	}

	template<typename T>
	inline bool ComponentManager::RemoveComponent(const EntityID aEntityID)
	{
		auto it = myComponentStorages.find(ComponentType(typeid(T)));

		if (it == myComponentStorages.end())
		{
			return false;
		}

		auto index = myEntityToComponentIndex[aEntityID].find(ComponentType(typeid(T)));

		if (index == myEntityToComponentIndex[aEntityID].end())
		{
			return false;
		}

		ComponentStorage<T>& componentStorage = *std::static_pointer_cast<ComponentStorage<T>>(it->second);
		componentStorage.Remove(index->second.index);

		myEntityToComponentIndex[aEntityID].erase(ComponentType(typeid(T)));
		myComponentTypeToEntityIDs[ComponentType(typeid(T))].erase(aEntityID);
		myEntityToComponentTypes[aEntityID].erase(ComponentType(typeid(T)));

		if (myEntityToComponentTypes[aEntityID].empty())
		{
			myEntityToComponentTypes.erase(aEntityID);
			myEntityToComponentIndex.erase(aEntityID);
		}

		return true;
	}

	template<typename T>
	inline T* ComponentManager::GetComponent(const EntityID aEntityID)
	{
		auto it = myEntityToComponentIndex.find(aEntityID);

		if (it != myEntityToComponentIndex.end())
		{
			ComponentStorage<T>& componentStorage = GetComponentStorage<T>();
			return componentStorage.GetComponentAtIndex(it->second[ComponentType(typeid(T))]);
		}

		return nullptr;
	}

	template<typename T>
	inline ComponentStorage<T>& ComponentManager::GetComponentStorage()
	{
		auto it = myComponentStorages.find(ComponentType(typeid(T)));

		if (it == myComponentStorages.end())
		{
			it = myComponentStorages.emplace(ComponentType(typeid(T)), std::make_shared<ComponentStorage<T>>()).first;
		}

		return *std::static_pointer_cast<ComponentStorage<T>>(it->second);
	}

	inline bool ComponentManager::RemoveAllComponents(const EntityID& aEntityID)
	{
		aEntityID;
	}

	inline bool ComponentManager::EntityHasComponent(const EntityID aEntityID, const ComponentType& aComponentType)
	{
		auto it = myEntityToComponentTypes.find(aEntityID);
		return it != myEntityToComponentTypes.end() && it->second.contains(aComponentType);
	}
}