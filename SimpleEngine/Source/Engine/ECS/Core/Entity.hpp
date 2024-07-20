#pragma once
#include "Engine/ECS/Core/EntityManager.hpp"
#include <string>
#include <typeindex>

namespace ECS
{
	class EntityManager;
}

namespace ECS
{
	class Entity final
	{
		friend class ECS::EntityManager;
	public:
		~Entity();

		Entity(const Entity& aOther) = default;
		Entity& operator=(const Entity& aOther) = default;
		Entity& operator=(Entity&& aOther) = default;

		template<typename T>
		const ComponentID AddComponent(const T& aValue = T());

		template<typename T>
		bool RemoveComponent();

		bool RemoveComponentByTypeIndex(const std::type_index& aTypeIndex);

		bool DestroyThis();

	public:
		void SetName(const std::string& aName);

		template<typename T>
		T* GetComponent();

		template<typename T>
		const T* GetComponent() const;

		const size_t GetID() const;
		const std::string& GetName() const;
		const std::unordered_map<ComponentType, ComponentID>& GetComponentMap() const;

	private:
		Entity(const size_t aID, EntityManager* aEntityManager);
	private:
		size_t myID;
		EntityManager* myEntityManager;
		std::string myName;
		char padding[8];
	};

	template<typename T>
	inline const ComponentID Entity::AddComponent(const T& aValue)
	{
		return myEntityManager->AddComponent<T>(myID, aValue);
	}

	template<typename T>
	inline bool Entity::RemoveComponent()
	{
		return myEntityManager->RemoveComponent<T>(myID);
	}

	template<typename T>
	inline T* Entity::GetComponent()
	{
		return myEntityManager->GetComponent<T>(myID);
	}

	template<typename T>
	inline const T* Entity::GetComponent() const
	{
		return myEntityManager->GetComponent<T>(myID);
	}
}