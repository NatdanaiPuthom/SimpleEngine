#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Core/EntityManager.hpp"
#include "Engine/ECS/Core/Entity.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"

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
		myAllEntityIDToIndexMap[myCurrentEntityID] = myAllEntities.size() - 1;

		return reinterpret_cast<Entity>(myEntities[myCurrentEntityID]);
	}

	bool EntityManager::DestroyEntity(const EntityID aID)
	{
		auto it = myEntities.find(aID);

		if (it != myEntities.end())
		{
			it->second = nullptr;
		}

		myAllEntities.erase(myAllEntities.begin() + myAllEntityIDToIndexMap[aID]);

		return false;
	}

	bool EntityManager::RemoveComponentByTypeName(const size_t aEntityID, const std::string& aComponentTypeName)
	{
		const std::type_index componentType = myComponentManager->GetComponentTypeIndexByName(aComponentTypeName);

		std::unordered_map<ComponentType, ComponentID>& components = myEntityComponents[aEntityID];
		auto it = components.find(componentType);

		if (it != components.end())
		{
			const ComponentID id = it->second;
			components.erase(it);

			return myComponentManager->RemoveComponentByTypeIndex(componentType, id);
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