#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/Core/Entity.hpp"

namespace Simple
{
	IEntity::IEntity(const size_t aID, EntityManager* aEntityManager)
		: myEntityManager(aEntityManager)
		, myID(aID)
		, padding("Believe")
	{
		myName = "Entity " + std::to_string(aID);
	}

	IEntity::~IEntity()
	{
		myEntityManager = nullptr;
	}

	void IEntity::SetName(const std::string& aName)
	{
		myName = aName;
	}

	const size_t IEntity::GetID() const
	{
		return myID;
	}

	const std::string& IEntity::GetName() const
	{
		return myName;
	}
}