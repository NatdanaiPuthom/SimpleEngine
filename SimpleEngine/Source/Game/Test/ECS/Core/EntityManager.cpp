#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/Core/EntityManager.hpp"
#include "Game/Test/ECS/Core/Entity.hpp"

namespace Simple
{
	size_t EntityManager::myCurrentEntityID = 0;
	ComponentManager* EntityManager::myComponentManager = nullptr;

	EntityManager::EntityManager(ComponentManager* aComponentManager)
	{
		myComponentManager = aComponentManager;;
	}

	void EntityManager::Init(const size_t aEntityAmountToReserved)
	{
		myEntityPool.Init(aEntityAmountToReserved);
	}

	EntityManager::~EntityManager()
	{
		myComponentManager = nullptr;
	}

	Entity EntityManager::CreateEntity()
	{
		myCurrentEntityID++;
		myEntities[myCurrentEntityID] = myEntityPool.CreateEntity(myCurrentEntityID, myEntities, this);
		myAllEntities.push_back(&myEntities[myCurrentEntityID]);

		return reinterpret_cast<IEntity*&>(myEntities[myCurrentEntityID]);
	}

	Entities EntityManager::GetAllEntities()
	{
		return Entities(myAllEntities);
	}
}