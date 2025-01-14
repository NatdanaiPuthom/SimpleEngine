#pragma once
#include "Engine/ECS/Core/ComponentManager.hpp"
#include <unordered_map>
#include <typeindex>
#include <vector>
#include <string>

namespace ECS
{
	class EntityComponentSystem;
	class Entity;
}

namespace ECS
{
	using EntityID = size_t;

	class EntityManager final
	{
		using ComponentType = std::type_index;
		friend class ECS::EntityComponentSystem;
	public:
		EntityManager(const EntityManager& aOther);
		EntityManager(EntityManager&& aOther) noexcept;
		EntityManager& operator=(const EntityManager& aOther);
		EntityManager& operator=(EntityManager&& aOther) noexcept;

		Entity& CreateEntity(EntityID aEntityID);
		bool DestroyEntity(const EntityID aID);

		//NOTE(v11.4.4): In case of different ECS instance
		EntityID DuplicateEntity(const Entity& aSourceEntity, const EntityManager* aSourceEntityManager);

		template<typename T>
		const ComponentID AddComponent(const EntityID aEntityID, const T& aValue);

		template<typename T>
		bool RemoveComponent(const EntityID aEntityID);

		bool RemoveComponentByTypeIndex(const ComponentType& aComponentType, const EntityID aEntityID);

	public:
		void* GetComponentPointerByComponentID(const ComponentID aComponentID);
		const void* GetComponentPointerByComponentID(const ComponentID aComponentID) const;

		template<typename T>
		T* GetComponent(const EntityID aEntityID);

		template<typename T>
		const T* GetComponent(const EntityID aEntityID) const;

		Entity& GetEntity(const EntityID aEntityID);
		std::vector<Entity>& GetAllEntities();

		const std::unordered_map<ComponentType, ComponentID>& GetComponentMap(const EntityID aEntityID) const;

	private:
		explicit EntityManager(ComponentManager* aComponentManager);
		~EntityManager();

		bool FindAndRemoveComponent(const ComponentType& aComponentType, const EntityID aEntityID);
	private:
		std::vector<Entity> myAllEntities;
		std::unordered_map<EntityID, size_t> myEntityIDToIndex;
		std::unordered_map<EntityID, size_t> myIndexToEntityID;
		std::unordered_map<EntityID, std::unordered_map<ComponentType, ComponentID>> myEntityComponents;

		ComponentManager* myComponentManager;
		size_t myCurrentEntityID;
		char myPadding[32] = "Never Give Up On Your Dream!\0";
	};

	template<typename T>
	inline const ComponentID EntityManager::AddComponent(const EntityID aEntityID, const T& aValue)
	{
		std::unordered_map<ComponentType, ComponentID>& components = myEntityComponents[aEntityID];

		if (components.contains(typeid(T)) == true)
		{
			return static_cast<ComponentID>(-1);
		}

		const ComponentID componentID = myComponentManager->CreateComponent<T>(aEntityID, aValue);

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
	inline T* EntityManager::GetComponent(const EntityID aEntityID)
	{
		if (myEntityComponents.contains(aEntityID) == false)
		{
			return nullptr;
		}

		const std::unordered_map<ComponentType, ComponentID>& components = myEntityComponents.at(aEntityID);

		auto it = components.find(typeid(T));

		if (it != components.end())
		{
			return myComponentManager->GetComponentByComponentID<T>(it->second);
		}

		return nullptr;
	}

	template<typename T>
	inline const T* EntityManager::GetComponent(const EntityID aEntityID) const
	{
		if (myEntityComponents.contains(aEntityID) == false)
		{
			return nullptr;
		}

		const std::unordered_map<ComponentType, ComponentID>& components = myEntityComponents.at(aEntityID);

		auto it = components.find(typeid(T));

		if (it != components.end())
		{
			return myComponentManager->GetComponentByComponentID<T>(it->second);
		}

		return nullptr;
	}
}