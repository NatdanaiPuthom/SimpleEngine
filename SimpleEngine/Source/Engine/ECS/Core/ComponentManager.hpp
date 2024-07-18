#pragma once
#include "Engine/ECS/MemoryPools/ComponentPool.hpp"
#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <cassert>

namespace ECS
{
	class EntityComponentSystem;
}

namespace ECS
{
	using ComponentID = size_t;
	using ComponentType = std::type_index;
	using EntityID = size_t;

	class ComponentManager final
	{
		friend class ECS::EntityComponentSystem;
	public:
		ComponentManager(const ComponentManager&&) = delete;
		ComponentManager& operator=(const ComponentManager&) = delete;
		ComponentManager& operator=(const ComponentManager&&) = delete;

		template<typename T>
		ComponentID CreateComponent(const EntityID aEntityID, const T& aComponent = T());

		bool RemoveComponentByTypeIndex(const ComponentType& aComponentType, const EntityID aEntityID, const ComponentID aComponentID);

	public:
		void* GetComponentByComponentID(const ComponentID aID);

		template<typename T>
		const std::unordered_set<EntityID>& GetEntityIDsWithThisComponent();

		template<typename T>
		T*& GetComponentByComponentID(const ComponentID aID);

	private:
		ComponentManager();
		~ComponentManager();
		ComponentManager(const ComponentManager& aOther);
	private:
		std::unordered_map<ComponentType, ComponentPool> myComponents;
		std::unordered_map<ComponentType, std::unordered_set<EntityID>> myComponentTypeToEntityIDs;
		std::unordered_map<ComponentID, ComponentType> myComponentIDToComponentTypeMap;
		size_t myCurrentComponentID;
		char myPadding[8] = "Believ\0";
	};

	template<typename T>
	inline ComponentID ComponentManager::CreateComponent(const EntityID aEntityID, const T& aComponent)
	{
		myCurrentComponentID++;
		myComponents[typeid(T)].CreateComponent<T>(myCurrentComponentID, aComponent);;
		myComponentTypeToEntityIDs[typeid(T)].insert(aEntityID);
		myComponentIDToComponentTypeMap.emplace(myCurrentComponentID, typeid(T));

		return myCurrentComponentID;
	}

	template<typename T>
	const std::unordered_set<EntityID>& ComponentManager::GetEntityIDsWithThisComponent()
	{
		return myComponentTypeToEntityIDs[typeid(T)];
	}

	template<typename T>
	inline T*& ComponentManager::GetComponentByComponentID(const ComponentID aID)
	{
		auto& componentIDToPointerMap = myComponents[typeid(T)].GetComponentIDToPointerMap();
		auto it = componentIDToPointerMap.find(aID);

		if (it != componentIDToPointerMap.end())
		{
			return reinterpret_cast<T*&>(it->second);
		}

		static T* nullPointer = nullptr;
		return std::ref(nullPointer);
	}
}