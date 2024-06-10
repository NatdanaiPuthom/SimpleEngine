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

		std::unordered_map<ComponentType, ComponentID>& components = myEntityComponents[aEntityID];

		auto component = components.find(aTypeIndex);

		if (component != components.end())
		{
			const ComponentID componentID = component->second;
			components.erase(component);
			componentID;
			return true;
			//return myComponentManager->RemoveComponentByTypeIndex(aTypeIndex, componentID); //TO-DO(v11.0.2): fix crash
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

		bool hasTransform = false;

		for (const auto& componentType : entityComponents)
		{
			std::string name = componentType.first.name();
			name = SimpleUtilities::ConvertTypeIndexNameToPrettyName(name);

			if (componentType.first == typeid(ECS::TransformComponent))
			{
				componentNames.insert(componentNames.begin(), name);
				hasTransform = true;
			}
			else if (componentType.first == typeid(ECS::MeshComponent))
			{
				if (hasTransform == true)
				{
					componentNames.insert(componentNames.begin() + 1, name);
				}
				else
				{
					componentNames.insert(componentNames.begin(), name);
				}
			}
			else
			{
				componentNames.push_back(name);
			}
		}

		return componentNames;
	}
}