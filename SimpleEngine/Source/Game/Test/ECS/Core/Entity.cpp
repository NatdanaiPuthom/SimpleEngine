#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/Core/Entity.hpp"

namespace Simple
{
	IEntity::IEntity(const size_t aID, EntityManager* aEntityManager)
		: myEntityManager(aEntityManager)
		, myID(aID)
	{
	}

	IEntity::~IEntity()
	{
		myEntityManager = nullptr;
	}

	const size_t IEntity::GetID() const
	{
		return myID;
	}
}