#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Core/EntityManager.hpp"
#include "Engine/ECS/Core/Entity.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"
#include "Engine/ECS/Components/Core/TransformComponent.hpp"
#include "Engine/ECS/Components/Core/MeshComponent.hpp"

namespace ECS
{
	EntityManager::EntityManager(ComponentManager* aComponentManager)
		: myCurrentEntityID(0)
		, myComponentManager(aComponentManager)
		, padding("Never Give Up!!")
	{
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

	bool EntityManager::DestroyEntity(const EntityID aID)
	{
		PROFILER_FUNCTION(profiler::colors::Red);
		ECS::Entities entities = GetAllEntities();
		const size_t count = entities.GetEntityCount();

		PROFILER_BEGIN("Search and remove entity");
		for (size_t i = 0; i < count; ++i) //TO-DO(v9.31.1): Faster remove algoritm as this is O(2) operation. I tried spatial half and half but got some error, may look into this in the future
		{
			if (entities[i]->GetID() == aID)
			{
				myAllEntities.erase(myAllEntities.begin() + i);
				break;
			}
		}
		PROFILER_END();

		auto it = myEntities.find(aID);
		if (it != myEntities.end())
		{
			PROFILER_BEGIN("Search and remove components from entity");
			std::unordered_map<ComponentType, ComponentID>& entityComponents = myEntityComponents[aID];

			for (const auto& [typeIndex, componentID] : entityComponents)
			{
				assert(myComponentManager->RemoveComponentByTypeIndex(typeIndex, componentID) && "Failed to remove component by type index");
			}

			entityComponents.clear();
			PROFILER_END();

			it->second = nullptr;

			return true;
		}

		return false;
	}

	bool EntityManager::RemoveComponentByTypeIndex(const std::type_index& aTypeIndex, const EntityID aEntityID)
	{
		if (myEntityComponents.contains(aEntityID) == false)
		{
			assert(false && "Entity with this ID does not exist");
			return false;
		}

		std::unordered_map<ComponentType, ComponentID>& entityComponents = myEntityComponents[aEntityID];

		auto component = entityComponents.find(aTypeIndex);

		if (component != entityComponents.end())
		{
			const ComponentID componentID = component->second;
			const bool removed = myComponentManager->RemoveComponentByTypeIndex(aTypeIndex, componentID);
			entityComponents.erase(component);

			return removed;
		}

		return false;
	}

	Entity EntityManager::GetEntity(const EntityID aEntityID)
	{
		return reinterpret_cast<Entity>(myEntities[aEntityID]);
	}

	Entities EntityManager::GetAllEntities()
	{
		return Entities(myAllEntities);
	}

	std::unordered_map<ComponentType, ComponentID>& EntityManager::GetComponentsMap(const EntityID aEntityID)
	{
		return myEntityComponents[aEntityID];
	}
}