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
	class IEntity final
	{
		friend class ECS::EntityManager;
	public:
		~IEntity();

		IEntity(const IEntity& aOther) = default;
		IEntity& operator=(const IEntity& aOther) = default;
		IEntity& operator=(IEntity&& aOther) = default;

		template<typename T>
		const ComponentID AddComponent();

		template<typename T>
		bool RemoveComponent();

		bool RemoveComponentByTypeIndex(const std::type_index& aTypeIndex);

		bool DestroyThis();

	public:
		void SetName(const std::string& aName);

		template<typename T>
		T*& GetComponent();

		const size_t GetID() const;
		const std::string& GetName() const;
		const std::unordered_map<ComponentType, ComponentID>& GetComponentMap() const;

	private:
		IEntity(const size_t aID, EntityManager* aEntityManager);
	private:
		size_t myID;
		EntityManager* myEntityManager;
		std::string myName;
		char padding[8];
	};

	template<typename T>
	inline const ComponentID IEntity::AddComponent()
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