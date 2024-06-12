#pragma once
#include "Engine/ECS/Core/ComponentManager.hpp"
#include "Engine/ECS/MemoryPools/EntityPool.hpp"
#include "Engine/ECS/Core/Entities.hpp"
#include <unordered_map>
#include <typeindex>
#include <vector>
#include <string>

namespace ECS
{
	class IEntity;
	class EntityComponentSystem;
}

namespace ECS
{
	using EntityID = size_t;

	class EntityManager final
	{
		using ComponentType = std::type_index;
		friend class ECS::EntityComponentSystem;
	public:
		Entity CreateEntity(EntityID aEntityID = 0);
		bool DestroyEntity(const EntityID aID);

		template<typename T>
		bool AddComponent(const EntityID aEntityID);

		template<typename T>
		bool RemoveComponent(const size_t aEntityID);

		bool RemoveComponentByTypeIndex(const std::type_index& aTypeIndex, const EntityID aEntityID);

	public:
		template<typename T>
		T*& GetComponent(const EntityID aEntityID);

		Entity GetEntity(const EntityID aEntityID);
		Entities GetAllEntities();

		const std::unordered_map<ComponentType, ComponentID>& GetComponentMap(const EntityID aEntityID);

	private:
		EntityManager(ComponentManager* aComponentManager);
		~EntityManager();

		void Init(const size_t aEntityAmountToReserved = 8);
	private:
		std::unordered_map<EntityID, char*> myEntities;
		std::unordered_map<EntityID, std::unordered_map<ComponentType, ComponentID>> myEntityComponents;
		std::vector<char**> myAllEntities;

		EntityPool myEntityPool;

		std::vector<EntityID> myRemovedEntityIDs;
		ComponentManager* myComponentManager;
		size_t myCurrentEntityID;
		const char padding[16];
	};

	template<typename T>
	inline bool EntityManager::AddComponent(const EntityID aEntityID)
	{
		std::unordered_map<ComponentType, ComponentID>& components = myEntityComponents[aEntityID];

		if (components.contains(typeid(T)) == true)
		{
			return false;
		}

		myEntityComponents[aEntityID][typeid(T)] = myComponentManager->CreateComponent<T>();
		return myEntityComponents[aEntityID].contains(typeid(T));
	}

	template<typename T>
	inline bool EntityManager::RemoveComponent(const size_t aEntityID)
	{
		std::unordered_map<ComponentType, ComponentID>& components = myEntityComponents[aEntityID];

		auto it = components.find(typeid(T));

		if (it != components.end())
		{
			const ComponentID id = it->second;
			components.erase(it);

			return myComponentManager->RemoveComponent<T>(id);
		}

		return false;
	}

	template<typename T>
	inline T*& EntityManager::GetComponent(const EntityID aEntityID)
	{
		std::unordered_map<ComponentType, ComponentID>& components = myEntityComponents[aEntityID];

		auto it = components.find(typeid(T));

		if (it != components.end())
		{
			return myComponentManager->GetComponentByComponentID<T>(it->second);
		}

		static T* nullPointer = nullptr;
		return std::ref(nullPointer);
	}
}