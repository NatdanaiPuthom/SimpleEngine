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
	{
	}

	EntityManager::~EntityManager()
	{
		myComponentManager = nullptr;
	}

	IEntity& EntityManager::CreateEntity(EntityID aEntityID)
	{
		if (aEntityID == 0)
		{
			aEntityID = myCurrentEntityID;
		}
		else if (aEntityID > myCurrentEntityID)
		{
			myCurrentEntityID = aEntityID;
		}

		myAllEntities.emplace_back(IEntity(aEntityID, this));

		const size_t index = myAllEntities.size() - 1;

		myEntityIDToIndex[aEntityID] = index;
		myIndexToEntityID[index] = aEntityID;

		++myCurrentEntityID;

		return myAllEntities.back();
	}

	bool EntityManager::DestroyEntity(const EntityID aEntityID)
	{
		const size_t entityToRemoveIndex = myEntityIDToIndex.at(aEntityID);
		const size_t lastEntityIndex = myAllEntities.size() - 1;
		const size_t lastEntityID = myIndexToEntityID.at(lastEntityIndex);

		myAllEntities[entityToRemoveIndex] = myAllEntities[lastEntityIndex];
		myAllEntities.pop_back();

		myIndexToEntityID[lastEntityIndex] = lastEntityID;
		myEntityIDToIndex[lastEntityID] = lastEntityIndex;

		myIndexToEntityID.erase(entityToRemoveIndex);
		myEntityIDToIndex.erase(aEntityID);

		const std::unordered_map<ComponentType, ComponentID>& entityComponents = myEntityComponents[aEntityID];

		for (const auto& [typeIndex, componentID] : entityComponents)
		{
			assert(myComponentManager->RemoveComponentByTypeIndex(typeIndex, aEntityID, componentID) && "Failed to remove component by type index");
		}

		return true;
	}

	bool EntityManager::FindAndRemoveComponent(const ComponentType& aComponentType, const EntityID aEntityID)
	{
		if (myEntityComponents.contains(aEntityID) == false)
		{
			assert(false && "Entity with this ID does not exist");
			return false;
		}

		std::unordered_map<ComponentType, ComponentID>& entityComponents = myEntityComponents[aEntityID];

		auto component = entityComponents.find(aComponentType);

		if (component != entityComponents.end())
		{
			const ComponentID componentID = component->second;
			const bool removed = myComponentManager->RemoveComponentByTypeIndex(aComponentType, aEntityID, componentID);
			entityComponents.erase(component);

			return removed;
		}

		return false;
	}

	bool EntityManager::RemoveComponentByTypeIndex(const ComponentType& aComponentType, const EntityID aEntityID)
	{
		return FindAndRemoveComponent(aComponentType, aEntityID);
	}

	IEntity& EntityManager::GetEntity(const EntityID aEntityID)
	{
		return myAllEntities[myEntityIDToIndex.at(aEntityID)];
	}

	std::vector<IEntity>& EntityManager::GetAllEntities()
	{
		return myAllEntities;
	}

	const std::unordered_map<ComponentType, ComponentID>& EntityManager::GetComponentMap(const EntityID aEntityID)
	{
		return myEntityComponents[aEntityID];
	}
}