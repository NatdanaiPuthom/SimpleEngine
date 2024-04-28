#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS2/EntityM.hpp"
#include "Game/Test/ECS2/EntityE.hpp"

namespace Simple
{
	size_t EntityM::myCurrentEntityID = 0;

	EntityM::EntityM(ComponentM* aComponentManager)
		: myComponentManager(aComponentManager)
	{
	}

	EntityM::~EntityM()
	{
		myComponentManager = nullptr;
	}

	EntityE*& EntityM::CreateEntity()
	{
		myCurrentEntityID++;
		myEntities[myCurrentEntityID] = myEntityPool.CreateEntity(myCurrentEntityID, myEntities, this);

		return reinterpret_cast<EntityE*&>(myEntities[myCurrentEntityID]);
	}
}