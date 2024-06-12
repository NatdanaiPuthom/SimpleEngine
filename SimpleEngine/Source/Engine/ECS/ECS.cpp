#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/ECS/Systems/RenderSystem.hpp"
#include "Engine/ECS/Reflection/ECSReflection.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"
#include "External/nlohmann/json.hpp"
#include <fstream>

namespace ECS
{
	EntityComponentSystem::EntityComponentSystem()
		: myEntityManager(&myComponentManager)
		, mySystemManager(&myEntityManager)
	{
	}

	EntityComponentSystem::~EntityComponentSystem()
	{
	}

	void EntityComponentSystem::LoadData()
	{
		const std::string filePath = SimpleUtilities::GetAbsolutePath("Assets/Scenes/scene_1.json"); filePath;
		const ECS::Entities entities = myEntityManager.GetAllEntities();

		nlohmann::ordered_json jsonData;

		for (size_t i = 0; i < entities.GetEntityCount(); ++i)
		{
			const Entity entity = entities[i];

			jsonData["Entities"][i]["ID"] = entity->GetID();;
			jsonData["Entities"][i]["Name"] = entity->GetName();;

			const std::unordered_map<ECS::ComponentType, ComponentID>& components = entity->GetComponentMap();
			size_t count = 0;

			for (const auto& [componentType, componentID] : components)
			{
				const std::string componentName = SimpleUtilities::ConvertTypeIndexNameToPrettyName(componentType.name());
				jsonData["Entities"][i]["Components"][count]["Name"] = componentName;

				/*void* componentPointer = myComponentManager.GetComponentByComponentID(componentID);
				std::vector<ComponentProperty>& componentProperties = ComponentRegistry::myTypeErasureComponents[componentType.hash_code()].myComponentProperties;
				auto unknownType = reinterpret_cast<void*>((reinterpret_cast<size_t>(componentPointer) + componentProperties[0].byteOffset));*/

				jsonData["Entities"][i]["Components"][count]["Properties"][ComponentRegistry::myTypeErasureComponents[componentType.hash_code()].myComponentProperties[0].name] = 0;
				++count;
			}
		}

		std::ofstream writeFile(filePath);
		assert(writeFile.is_open() && "Failed to open the file");

		writeFile << jsonData.dump(-1);
		writeFile.close();
	}

	void EntityComponentSystem::Init()
	{
		constexpr size_t entitiesToReserve = 8; //NOTE(v9.30.10):Small number for experimental purposes for now
		myEntityManager.Init(entitiesToReserve);

		mySystemManager.AddSystem(std::move(std::make_unique<RenderSystem>(&myEntityManager)));

		mySystemManager.Init();
	}

	void EntityComponentSystem::Update()
	{
		mySystemManager.Update();
	}

	void EntityComponentSystem::Render()
	{
		mySystemManager.Render();
	}

	void EntityComponentSystem::RenderPointLights()
	{
		mySystemManager.RenderPointLights();
	}

	void EntityComponentSystem::RenderSkyBoxAndDirectionalLight()
	{
		mySystemManager.RenderSkyBoxAndDirectionalLight();
	}

	void EntityComponentSystem::AddSystem(std::unique_ptr<System> aSystem)
	{
		mySystemManager.AddSystem(std::move(aSystem));
	}

	Entity EntityComponentSystem::CreateEntity(const EntityID aEntityID)
	{
		return myEntityManager.CreateEntity(aEntityID);
	}

	bool EntityComponentSystem::RemoveEntity(const EntityID aEntityID)
	{
		return myEntityManager.DestroyEntity(aEntityID);
	}

	void EntityComponentSystem::SetGlobalPointerToThis()
	{
		Impl::SimpleWorldECS::SetECS(this);
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
}