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
		bool AddComponent(const EntityID aEntityID);

		template<typename T>
		bool RemoveComponent(const size_t aEntityID);

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
	inline bool EntityManager::AddComponent(const EntityID aEntityID)
	{
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
		return myComponentManager->GetComponentByComponentID<T>(myEntityComponents[aEntityID][typeid(T)]);
	}
}