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