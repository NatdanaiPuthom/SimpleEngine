#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS2/EntityE.hpp"

namespace Simple
{
	EntityE::EntityE(const size_t aID, EntityM* aEntityManager)
		: myEntityManager(aEntityManager)
		, myID(aID)
	{
	}

	EntityE::~EntityE()
	{
		myEntityManager = nullptr;
	}

	size_t EntityE::GetID() const
	{
		return myID;
	}
}