#pragma once
#include "Engine/ECS/Core/EntityManager.hpp"
#include "Engine/ECS/ECSInterface.hpp"
#include <string>

namespace ECS
{
	class EntityPool;
	class EntityManager;
}

namespace ECS
{
	class IEntity final
	{
		friend class ECS::EntityManager;
		friend class ECS::EntityPool;
	public:
		~IEntity();

		template<typename T>
		bool AddComponent();

		template<typename T>
		bool RemoveComponent();

		bool RemoveComponentByTypeIndex(const std::type_index& aTypeIndex);

		bool DestroyThis();

		void SetName(const std::string& aName);

		template<typename T>
		T*& GetComponent();

		const size_t GetID() const;
		const std::string& GetName() const;
		const std::vector<std::string> GetComponentNames() const;

		inline std::unordered_map<ComponentType, ComponentID>& GetComponentMap()
		{
			return myEntityManager->GetComponentsMap(myID);
		}

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