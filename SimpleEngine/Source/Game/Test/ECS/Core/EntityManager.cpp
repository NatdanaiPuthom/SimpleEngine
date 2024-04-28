#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/Core/EntityManager.hpp"
#include "Game/Test/ECS/Core/Entity.hpp"

namespace Simple
{
	size_t EntityManager::myCurrentEntityID = 0;

	EntityManager::EntityManager(ComponentManager* aComponentManager)
		: myComponentManager(aComponentManager)
	{
	}

	void EntityManager::Init(const size_t aEntityAmountToReserved)
	{
		myEntityPool.Init(aEntityAmountToReserved);
	}

	EntityManager::~EntityManager()
	{
		myComponentManager = nullptr;
	}

	Entity*& EntityManager::CreateEntity()
	{
		myCurrentEntityID++;
		myEntities[myCurrentEntityID] = myEntityPool.CreateEntity(myCurrentEntityID, myEntities, this);

		return reinterpret_cast<Entity*&>(myEntities[myCurrentEntityID]);
	}
}