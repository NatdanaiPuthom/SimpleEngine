#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/Core/Entity.hpp"

namespace Simple
{
	Entity::Entity(const size_t aID, EntityManager* aEntityManager)
		: myEntityManager(aEntityManager)
		, myID(aID)
	{
	}

	Entity::~Entity()
	{
		myEntityManager = nullptr;
	}

	const size_t Entity::GetID() const
	{
		return myID;
	}
}