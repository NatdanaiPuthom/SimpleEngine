#pragma once
#include "Game/Test/ECS/Core/EntityManager.hpp"
#include "Game/Test/ECS/ECSInterface.hpp"
#include <string>

namespace Simple
{
	class EntityPool;
	class EntityManager;
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
		bool AddComponent();

		template<typename T>
		bool RemoveComponent();

		void SetName(const std::string& aName);

		template<typename T>
		T*& GetComponent();

		const size_t GetID() const;
		const std::string& GetName() const;
		const std::vector<std::string> GetComponentNames() const;

	private:
		IEntity(const size_t aID, EntityManager* aEntityManager);
	private:
		const size_t myID;
		EntityManager* myEntityManager;
		std::string myName;
		const char padding[8];
	};

	template<typename T>
	inline bool IEntity::AddComponent()
	{
		return myEntityManager->AddComponent<T>(myID);
	}

	template<typename T>
	inline bool IEntity::RemoveComponent()
	{
		return myEntityManager->RemoveComponent<T>(myID);
	}

	template<typename T>
	inline T*& IEntity::GetComponent()
	{
		return myEntityManager->GetComponent<T>(myID);
	}
}