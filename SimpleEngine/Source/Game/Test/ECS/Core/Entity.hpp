#pragma once
#include "Game/Test/ECS/Core/EntityManager.hpp"
#include "Game/Test/ECS/ECSInterface.hpp"

namespace Simple
{
	class EntityPool;
}

namespace Simple
{
	class IEntity final
	{
		friend class Simple::EntityManager;
		friend class Simple::EntityPool;
	public:

		~IEntity();

		template<typename T>
		inline void AddComponent();

		template<typename T>
		inline T*& GetComponent();

		const size_t GetID() const;

	private:
		IEntity(const size_t aID, EntityManager* aEntityManager);
	private:
		const size_t myID;
		EntityManager* myEntityManager;
	};

	template<typename T>
	inline void IEntity::AddComponent()
	{
		myEntityManager->AddComponent<T>(myID);
	}

	template<typename T>
	inline T*& IEntity::GetComponent()
	{
		return myEntityManager->GetComponent<T>(myID);
	}
}