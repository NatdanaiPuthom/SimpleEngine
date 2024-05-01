#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/Core/EntityManager.hpp"
#include "Game/Test/ECS/Core/Entity.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"

namespace Simple
{
	size_t EntityManager::myCurrentEntityID = 0;
	ComponentManager* EntityManager::myComponentManager = nullptr;

	EntityManager::EntityManager(ComponentManager* aComponentManager)
	{
		myComponentManager = aComponentManager;;
	}

	void EntityManager::Init(const size_t aEntityAmountToReserved)
	{
		myEntityPool.Init(aEntityAmountToReserved);
	}

	EntityManager::~EntityManager()
	{
		myComponentManager = nullptr;
	}

	Entity EntityManager::CreateEntity()
	{
		myCurrentEntityID++;
		myEntities[myCurrentEntityID] = myEntityPool.CreateEntity(myCurrentEntityID, myEntities, this);
		myAllEntities.push_back(&myEntities[myCurrentEntityID]);

		return reinterpret_cast<Entity>(myEntities[myCurrentEntityID]);
	}

	Entity EntityManager::GetEntity(const EntityID aEntityID)
	{
		return reinterpret_cast<Entity>(myEntities[aEntityID]);
	}

	Entities EntityManager::GetAllEntities()
	{
		return Entities(myAllEntities);
	}

	const std::vector<std::string> EntityManager::GetComponentNames(const EntityID aEntityID)
	{
		std::unordered_map<ComponentType, ComponentID>& entityComponents = myEntityComponents[aEntityID];
		std::vector<std::string> componentNames;
		componentNames.reserve(entityComponents.size());

		for (const auto& componentType : entityComponents)
		{
			std::string name = componentType.first.name();
			name = SimpleUtilities::ConvertTypeIndexNameToPrettyName(name);
			componentNames.push_back(name);
		}

		return componentNames;
	}
}