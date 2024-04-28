#pragma once
#include "Game/Test/ECS/Core/EntityManager.hpp"

namespace Simple
{
	class EntityPool;
}

namespace Simple
{
	class Entity final
	{
		friend class Simple::EntityManager;
		friend class Simple::EntityPool;
	public:
		~Entity();

		template<typename T>
		inline void AddComponent();

		template<typename T>
		inline T*& GetComponent();

		const size_t GetID() const;

	private:
		Entity(const size_t aID, EntityManager* aEntityManager);
	private:
		const size_t myID;
		EntityManager* myEntityManager;
	};

	template<typename T>
	inline void Entity::AddComponent()
	{
		myEntityManager->AddComponent<T>(myID);
	}

	template<typename T>
	inline T*& Entity::GetComponent()
	{
		return myEntityManager->GetComponent<T>(myID);
	}
}