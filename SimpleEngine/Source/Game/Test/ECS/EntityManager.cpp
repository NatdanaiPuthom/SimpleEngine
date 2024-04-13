#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/EntityManager.hpp"

namespace Simple
{
	EntityManager::EntityManager()
	{
	}

	EntityManager::~EntityManager()
	{
	}

	Entity& EntityManager::CreateEntity()
	{
		myEntities.push_back(Simple::Entity());

		const size_t entityID = myEntities.size() - 1;

		myEntityMapToPointer[entityID] = &myEntities[entityID];
		myEntityMapToID[&myEntities.back()] = entityID;

		return myEntities[entityID];
	}

	Entity* EntityManager::GetEntity(const size_t aEntityID) const
	{
		auto it = myEntityMapToPointer.find(aEntityID);

		if (it != myEntityMapToPointer.end())
		{
			return it->second;
		}

		return nullptr;
	}

	size_t EntityManager::GetEntityID(Entity* aEntity)
	{
		if (aEntity == nullptr)
		{
			assert(false && "Could not find Entity");
			return static_cast<size_t>(-1);
		}

		auto it = myEntityMapToID.find(aEntity);

		if (it != myEntityMapToID.end())
		{
			return myEntityMapToID[aEntity];
		}
		else
		{
			assert(false && "Could not find Entity");
			return static_cast<size_t>(-1);
		}
	}

	const std::vector<Entity>& EntityManager::GetAllEntities() const
	{
		return myEntities;
	}

	std::vector<Entity>& EntityManager::GetAllEntities()
	{
		return myEntities;
	}
}