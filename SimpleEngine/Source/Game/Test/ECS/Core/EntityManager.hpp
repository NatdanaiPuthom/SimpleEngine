#pragma once
#include "Game/Test/ECS/Core/ComponentManager.hpp"
#include "Game/Test/ECS/MemoryPools/EntityPool.hpp"
#include "Game/Test/ECS/Core/Entities.hpp"
#include <unordered_map>
#include <typeindex>
#include <vector>
#include <string>

namespace Simple
{
	class IEntity;
	class ECS;
}

namespace Simple
{
	using EntityID = size_t;

	class EntityManager final
	{
		using ComponentType = std::type_index;
		friend class Simple::ECS;
	public:
		Entity CreateEntity();

		template<typename T>
		void AddComponent(const EntityID aEntityID);

		template<typename T>
		T*& GetComponent(const EntityID aEntityID);

		Entity GetEntity(const EntityID aEntityID);
		Entities GetAllEntities();
		const std::vector<std::string> GetComponentNames(const EntityID aEntityID);

	private:
		EntityManager(ComponentManager* aComponentManager);
		~EntityManager();

		void Init(const size_t aEntityAmountToReserved = 8);
	private:
		static size_t myCurrentEntityID;
		static ComponentManager* myComponentManager;
		EntityPool myEntityPool;
		std::unordered_map<EntityID, std::unordered_map<ComponentType, ComponentID>> myEntityComponents;
		std::unordered_map<EntityID, char*> myEntities;
		std::vector<char**> myAllEntities;
	};

	template<typename T>
	inline void EntityManager::AddComponent(const EntityID aEntityID)
	{
		myEntityComponents[aEntityID][typeid(T)] = myComponentManager->CreateComponent<T>();
	}

	template<typename T>
	inline T*& EntityManager::GetComponent(const EntityID aEntityID)
	{
		return myComponentManager->GetComponentByComponentID<T>(myEntityComponents[aEntityID][typeid(T)]);
	}
}