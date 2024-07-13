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
		const ComponentID AddComponent(const EntityID aEntityID);

		template<typename T>
		bool RemoveComponent(const EntityID aEntityID);

		bool RemoveComponentByTypeIndex(const ComponentType& aComponentType, const EntityID aEntityID);

	public:
		template<typename T>
		T*& GetComponent(const EntityID aEntityID);

		Entity GetEntity(const EntityID aEntityID);
		Entities GetAllEntities();

		const std::unordered_map<ComponentType, ComponentID>& GetComponentMap(const EntityID aEntityID);

	private:
		explicit EntityManager(ComponentManager* aComponentManager);
		~EntityManager();

		void Init(const size_t aEntityAmountToReserved = 8);
		bool FindAndRemoveComponent(const ComponentType& aComponentType, const EntityID aEntityID);
	private:
		std::unordered_map<EntityID, char*> myEntities;
		std::unordered_map<EntityID, std::unordered_map<ComponentType, ComponentID>> myEntityComponents;
		std::vector<char**> myAllEntities;

		EntityPool myEntityPool;

		ComponentManager* myComponentManager;
		size_t myCurrentEntityID;
		char padding[48];
	};

	template<typename T>
	inline const ComponentID EntityManager::AddComponent(const EntityID aEntityID)
	{
		std::unordered_map<ComponentType, ComponentID>& components = myEntityComponents[aEntityID];

		if (components.contains(typeid(T)) == true)
		{
			return static_cast<ComponentID>(-1);
		}

		const ComponentID componentID = myComponentManager->CreateComponent<T>(aEntityID);

		myEntityComponents[aEntityID][typeid(T)] = componentID;

		if (myEntityComponents[aEntityID].contains(typeid(T)))
		{
			return componentID;
		}

		return static_cast<ComponentID>(-1);
	}

	template<typename T>
	inline bool EntityManager::RemoveComponent(const EntityID aEntityID)
	{
		return FindAndRemoveComponent(typeid(T), aEntityID);
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