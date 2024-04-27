#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS2/EntityM.hpp"
#include "Game/Test/ECS2/EntityE.hpp"

namespace Simple
{
	EntityM::EntityM(ComponentM* aComponentManager)
		: myComponentManager(aComponentManager)
	{
	}

	EntityM::~EntityM()
	{
		myComponentManager = nullptr;
	}

	EntityE& EntityM::CreateEntity()
	{
		return myEntities.emplace_back(EntityE(2, this));
	}
}