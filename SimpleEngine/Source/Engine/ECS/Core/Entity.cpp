#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Core/Entity.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace ECS
{
	Entity::Entity(const size_t aID, EntityManager* aEntityManager)
		: myEntityManager(aEntityManager)
		, myID(aID)
		, padding("Believe")
	{
		myName = "Entity " + std::to_string(aID);
	}

	Entity::~Entity()
	{
		myEntityManager = nullptr;
	}

	bool Entity::RemoveComponentByTypeIndex(const std::type_index& aTypeIndex)
	{
		return myEntityManager->RemoveComponentByTypeIndex(aTypeIndex, myID);
	}

	bool Entity::DestroyThis()
	{
		return myEntityManager->DestroyEntity(myID);
	}

	EntityID Entity::DuplicateThisToAnotherECS(EntityManager* aDestinationEntityManager) const
	{
		return aDestinationEntityManager->DuplicateEntity(*this, myEntityManager);
	}

	EntityID Entity::DuplicateThis() const
	{
		return myEntityManager->DuplicateEntity(*this, myEntityManager);
	}

	void Entity::SetName(const std::string& aName)
	{
		myName = aName;
	}

	const size_t Entity::GetID() const
	{
		return myID;
	}

	const std::string& Entity::GetName() const
	{
		return myName;
	}

	const std::unordered_map<ComponentType, ComponentID>& Entity::GetComponentMap() const
	{
		return myEntityManager->GetComponentMap(myID);
	}
}