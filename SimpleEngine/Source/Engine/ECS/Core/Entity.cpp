#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Core/Entity.hpp"

namespace ECS
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

	bool IEntity::RemoveComponentByTypeIndex(const std::type_index& aTypeIndex)
	{
		return myEntityManager->RemoveComponentByTypeIndex(aTypeIndex, myID);
	}

	bool IEntity::DestroyThis()
	{
		return myEntityManager->DestroyEntity(myID);
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

	const std::vector<std::string> IEntity::GetComponentNames() const
	{
		return myEntityManager->GetComponentNames(myID);
	}
}