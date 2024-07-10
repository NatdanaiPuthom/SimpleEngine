#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/ECS/Systems/RenderSystem.hpp"
#include "MainSingleton/MainSingleton.hpp"
#include "Game/WorldImpl/SimpleWorldImpl.hpp"
#include "External/nlohmann/json.hpp"
#include <fstream>

#include "Engine/ECS/Systems/TestSystem.hpp"

namespace ECS
{
	EntityComponentSystem::EntityComponentSystem()
		: myEntityManager(&myComponentManager)
		, mySystemManager(this)
	{
	}

	EntityComponentSystem::~EntityComponentSystem()
	{
	}

	void EntityComponentSystem::Init()
	{
		constexpr size_t entitiesToReserve = 16; //NOTE(v9.30.10):Small number for experimental purposes for now
		myEntityManager.Init(entitiesToReserve);

		mySystemManager.AddSystem<TestSystem>(this);

		mySystemManager.Init();
	}

	void EntityComponentSystem::Update()
	{
		mySystemManager.Update();
	}

	void EntityComponentSystem::UpdateRenderSystem()
	{
		mySystemManager.UpdateRenderSystem();
	}

	void EntityComponentSystem::Render()
	{
		mySystemManager.Render();
	}

	void EntityComponentSystem::EarlyUpdate()
	{
		mySystemManager.EarlyUpdate();
	}

	void EntityComponentSystem::FixedUpdate()
	{
		mySystemManager.FixedUpdate();
	}

	void EntityComponentSystem::LateUpdate()
	{
		mySystemManager.LateUpdate();
	}

	void EntityComponentSystem::RenderPointLights()
	{
		mySystemManager.RenderPointLights();
	}

	void EntityComponentSystem::RenderSkyBoxAndDirectionalLight()
	{
		mySystemManager.RenderSkyBoxAndDirectionalLight();
	}

	Entity EntityComponentSystem::CreateEntity(const EntityID aEntityID)
	{
		return myEntityManager.CreateEntity(aEntityID);
	}

	bool EntityComponentSystem::RemoveEntity(const EntityID aEntityID)
	{
		return myEntityManager.DestroyEntity(aEntityID);
	}

	Entity EntityComponentSystem::GetEntity(const EntityID aID)
	{
		return myEntityManager.GetEntity(aID);
	}

	Entities EntityComponentSystem::GetAllEntities()
	{
		return myEntityManager.GetAllEntities();
	}

	void* EntityComponentSystem::GetComponentPointerByComponentID(const ComponentID aComponentID)
	{
		return myComponentManager.GetComponentByComponentID(aComponentID);
	}

	void EntityComponentSystem::SaveData(EntityComponentSystem& aECS, const std::string& aFileName)
	{
		const ECS::Entities entities = aECS.GetAllEntities();

		nlohmann::ordered_json jsonData;

		for (size_t i = 0; i < entities.GetEntityCount(); ++i)
		{
			const Entity entity = entities[i];
			const std::unordered_map<ECS::ComponentType, ComponentID>& components = entity->GetComponentMap();

			jsonData["Entities"][i]["ID"] = entity->GetID();;
			jsonData["Entities"][i]["Name"] = entity->GetName();;

			size_t count = 0;
			for (const auto& [componentType, componentID] : components)
			{
				void* componentPointer = aECS.GetComponentPointerByComponentID(componentID);
				const std::vector<ComponentProperty>& componentProperties = MainSingleton::GetComponentRegistry()->myTypeErasureComponents[componentType.hash_code()].myComponentProperties;

				const std::string componentName = SimpleUtilities::ConvertTypeIndexNameToPrettyName(componentType.name());
				jsonData["Entities"][i]["Components"][count]["Name"] = componentName;

				for (const ECS::ComponentProperty& componentProperty : componentProperties)
				{
					const std::string& propertyName = componentProperty.name;
					const size_t propertyID = componentProperty.id;
					const size_t byteOffset = componentProperty.byteOffset;

					const TypeErasureObject& dataType = MainSingleton::GetComponentRegistry()->myTypeErasureDataTypes[propertyID];

					jsonData["Entities"][i]["Components"][count]["Properties"][propertyName] = INT_MIN;

					if (const auto& getDataFunction = dataType.GetDataAsJSON)
					{
						void* propertyPointer = reinterpret_cast<void*>((reinterpret_cast<size_t>(componentPointer) + byteOffset));
						const nlohmann::json json = getDataFunction(propertyPointer, propertyName);
						jsonData["Entities"][i]["Components"][count]["Properties"][propertyName] = json[propertyName];
					}
				}

				++count;
			}
		}

		const std::string filePath = SimpleUtilities::GetAbsolutePath(aFileName);
		std::ofstream writeFile(filePath);
		assert(writeFile.is_open() && "Failed to open the file");

		writeFile << jsonData.dump(-1);
		writeFile.close();
	}

	void EntityComponentSystem::LoadData(EntityComponentSystem& aECS, const std::string& aFileName)
	{
		const std::string filePath = SimpleUtilities::GetAbsolutePath(aFileName);
		std::ifstream readFile(filePath);
		assert(readFile.is_open() && "Failed to open the file");

		const nlohmann::json jsonData = nlohmann::json::parse(readFile);
		readFile.close();

		if (jsonData.contains("Entities") == false)
		{
			return;
		}

		for (const auto& entityData : jsonData["Entities"])
		{
			const EntityID id = entityData["ID"];
			const std::string name = entityData["Name"];

			Entity entity = aECS.CreateEntity(id);
			entity->SetName(name);

			if (entityData.contains("Components") == false)
			{
				continue;
			}

			ECS::ComponentRegistry* componentRegistry = MainSingleton::GetComponentRegistry();

			for (const auto& componentDataJSON : entityData["Components"])
			{
				const size_t componentHashCode = componentRegistry->myComponentNameToHashCode[componentDataJSON["Name"]];
				const std::vector<ComponentProperty>& componentProperties = componentRegistry->myTypeErasureComponents[componentHashCode].myComponentProperties;
				const ComponentID componentID = componentRegistry->myTypeErasureComponents[componentHashCode].AddComponentFunctionPointer(entity);
				const auto& componentPropertiesJSON = componentDataJSON["Properties"];
				const size_t propertySize = componentPropertiesJSON.size();

				void* componentPointer = aECS.GetComponentPointerByComponentID(componentID);

				if (componentProperties.size() != propertySize)
				{
					assert(false && "Registered Component Properties doesn't match between Component and JSON data");
					continue;
				}

				for (size_t i = 0; i < propertySize; ++i)
				{
					const ComponentProperty& property = componentProperties[i];
					const TypeErasureObject& typeErasedData = componentRegistry->myTypeErasureDataTypes[property.id];
					void* propertyPointer = reinterpret_cast<void*>((reinterpret_cast<size_t>(componentPointer) + property.byteOffset));

					if (const auto& loadDataFromJSONFunction = typeErasedData.LoadDataFromJSON)
					{
						loadDataFromJSONFunction(propertyPointer, property.name, componentPropertiesJSON);
					}
				}
			}
		}
	}

}