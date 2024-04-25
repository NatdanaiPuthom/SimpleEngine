#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS2/EntityM.hpp"

namespace Simple
{
	EntityM::EntityM()
	{
	}

	EntityM::~EntityM()
	{
	}

	EntityE& EntityM::CreateEntity()
	{
		return myEntities.emplace_back(EntityE(2));
	}




	EntityE::EntityE(int b)
		: a(b)
	{
	}
}