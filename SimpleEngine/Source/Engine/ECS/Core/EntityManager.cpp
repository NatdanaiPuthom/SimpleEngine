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


	EntityManager::EntityManager(const EntityManager& aOther)
		: myAllEntities(aOther.myAllEntities)
		, myEntityIDToIndex(aOther.myEntityIDToIndex)
		, myIndexToEntityID(aOther.myIndexToEntityID)
		, myEntityComponents(aOther.myEntityComponents)
		, myComponentManager(aOther.myComponentManager)
		, myCurrentEntityID(aOther.myCurrentEntityID)
	{
		for (size_t i = 0; i < myAllEntities.size(); i++)
		{
			myAllEntities[i].myEntityManager = this;
		}
	}

	EntityManager::EntityManager(EntityManager&& aOther) noexcept
		: myAllEntities(std::move(aOther.myAllEntities))
		, myEntityIDToIndex(std::move(aOther.myEntityIDToIndex))
		, myIndexToEntityID(std::move(aOther.myIndexToEntityID))
		, myEntityComponents(std::move(aOther.myEntityComponents))
		, myComponentManager(aOther.myComponentManager)
		, myCurrentEntityID(aOther.myCurrentEntityID)
	{
		for (size_t i = 0; i < myAllEntities.size(); i++)
		{
			myAllEntities[i].myEntityManager = this;
		}

		aOther.myComponentManager = nullptr;
		aOther.myCurrentEntityID = 0;
	}

	EntityManager& EntityManager::operator=(const EntityManager& aOther)
	{
		this->myAllEntities = aOther.myAllEntities;
		this->myEntityIDToIndex = aOther.myEntityIDToIndex;
		this->myIndexToEntityID = aOther.myIndexToEntityID;
		this->myEntityComponents = aOther.myEntityComponents;
		this->myComponentManager = aOther.myComponentManager;
		this->myCurrentEntityID = aOther.myCurrentEntityID;

		for (size_t i = 0; i < myAllEntities.size(); i++)
		{
			myAllEntities[i].myEntityManager = this;
		}

		return *this;
	}

	EntityManager& EntityManager::operator=(EntityManager&& aOther) noexcept
	{
		this->myAllEntities = std::move(aOther.myAllEntities);
		this->myEntityIDToIndex = std::move(aOther.myEntityIDToIndex);
		this->myIndexToEntityID = std::move(aOther.myIndexToEntityID);
		this->myEntityComponents = std::move(aOther.myEntityComponents);
		this->myComponentManager = aOther.myComponentManager;
		this->myCurrentEntityID = aOther.myCurrentEntityID;

		for (size_t i = 0; i < myAllEntities.size(); i++)
		{
			myAllEntities[i].myEntityManager = this;
		}

		aOther.myComponentManager = nullptr;
		aOther.myCurrentEntityID = 0;

		return *this;
	}

	Entity& EntityManager::CreateEntity(EntityID aEntityID)
	{
		if (aEntityID == 0)
		{
			aEntityID = myCurrentEntityID;
		}
		else if (aEntityID > myCurrentEntityID)
		{
			myCurrentEntityID = aEntityID;
		}

		myAllEntities.emplace_back(Entity(aEntityID, this));

		const size_t index = myAllEntities.size() - 1;

		myEntityIDToIndex[aEntityID] = index;
		myIndexToEntityID[index] = aEntityID;

		++myCurrentEntityID;

		return myAllEntities.back();
	}

	bool EntityManager::DestroyEntity(const EntityID aEntityID)
	{
		std::unordered_map<ComponentType, ComponentID>& entityComponents = myEntityComponents.at(aEntityID);

		for (const auto& [typeIndex, componentID] : entityComponents)
		{
			[[maybe_unused]] const bool success = myComponentManager->RemoveComponentByTypeIndex(typeIndex, aEntityID, componentID);
			assert(success && "Failed to remove component by type index"); //TO-DO(v11.4.3): Make a Logger instead
		}

		entityComponents.clear();
		myEntityComponents.erase(aEntityID);

		size_t entityToRemoveIndex1 = myEntityIDToIndex.at(aEntityID);
		size_t entityToRemoveID1 = aEntityID;

		size_t entityToReplaceIndex1 = myAllEntities.size() - 1;
		size_t entityToReplaceID1 = myIndexToEntityID.at(entityToReplaceIndex1);

		myAllEntities[entityToRemoveIndex1] = std::move(myAllEntities[entityToReplaceIndex1]);
		myAllEntities.pop_back();

		myIndexToEntityID[entityToRemoveIndex1] = entityToReplaceID1;
		myEntityIDToIndex[entityToReplaceID1] = entityToRemoveIndex1;

		myIndexToEntityID.erase(entityToReplaceIndex1);
		myEntityIDToIndex.erase(entityToRemoveID1);

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

	Entity& EntityManager::GetEntity(const EntityID aEntityID)
	{
		return myAllEntities[myEntityIDToIndex.at(aEntityID)];
	}

	std::vector<Entity>& EntityManager::GetAllEntities()
	{
		return myAllEntities;
	}

	const std::unordered_map<ComponentType, ComponentID>& EntityManager::GetComponentMap(const EntityID aEntityID)
	{
		return myEntityComponents[aEntityID];
	}
}