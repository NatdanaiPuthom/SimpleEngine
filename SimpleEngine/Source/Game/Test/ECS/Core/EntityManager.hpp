#pragma once
#include "Game/Test/ECS/Core/ComponentManager.hpp"
#include "Game/Test/ECS/MemoryPools/EntityPool.hpp"
#include <unordered_map>
#include <typeindex>
#include <vector>

namespace Simple
{
	class Entity;
	class ECS;
}

namespace Simple
{
	class EntityManager final
	{
		using EntityID = size_t;
		using ComponentType = std::type_index;

		friend class Simple::ECS;
	public:
		~EntityManager();

		Entity*& CreateEntity();
		
		template<typename T>
		void AddComponent(const size_t aEntityID);

		template<typename T>
		T*& GetComponent(const size_t aEntityID);

	private:
		EntityManager(ComponentManager* aComponentManager);
	private:
		static size_t myCurrentEntityID;
		EntityPool myEntityPool;
		ComponentManager* myComponentManager;
		std::unordered_map<EntityID, std::unordered_map<ComponentType, ComponentID>> myEntityComponents;
		std::unordered_map<EntityID, char*> myEntities;
	};

	template<typename T>
	inline void EntityManager::AddComponent(const size_t aEntityID)
	{
		myEntityComponents[aEntityID][typeid(T)] = myComponentManager->CreateComponent<T>();
	}

	template<typename T>
	inline T*& EntityManager::GetComponent(const size_t aEntityID)
	{
		return myComponentManager->GetComponentByComponentID<T>(myEntityComponents[aEntityID][typeid(T)]);
	}
}