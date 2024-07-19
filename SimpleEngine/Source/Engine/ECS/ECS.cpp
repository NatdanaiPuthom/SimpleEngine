#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/ECS/Systems/RenderSystem.hpp"
#include "Engine/Debugger/Console/Console.hpp"
#include "MainSingleton/MainSingleton.hpp"
#include "External/nlohmann/json.hpp"
#include <fstream>

namespace ECS
{
	EntityComponentSystem::EntityComponentSystem()
		: myEntityManager(&myComponentManager)
		, mySystemManager()
	{
	}

	EntityComponentSystem::~EntityComponentSystem()
	{
	}

	EntityComponentSystem::EntityComponentSystem(const EntityComponentSystem& aOther)
		: myEntityManager(aOther.myEntityManager)
		, myComponentManager(aOther.myComponentManager)
		, mySystemManager(aOther.mySystemManager)
	{
		myEntityManager.myComponentManager = &myComponentManager;
	}


	EntityComponentSystem::EntityComponentSystem(EntityComponentSystem&& aOther) noexcept
		: myEntityManager(std::move(aOther.myEntityManager))
		, myComponentManager(std::move(myComponentManager))
		, mySystemManager(std::move(aOther.mySystemManager))
	{
		myEntityManager.myComponentManager = &myComponentManager;
	}

	EntityComponentSystem& EntityComponentSystem::operator=(const EntityComponentSystem& aOther)
	{
		myEntityManager = aOther.myEntityManager;
		myComponentManager = aOther.myComponentManager;
		mySystemManager = aOther.mySystemManager;

		myEntityManager.myComponentManager = &myComponentManager;

		return *this;
	}

	EntityComponentSystem& EntityComponentSystem::operator=(EntityComponentSystem&& aOther) noexcept
	{
		myEntityManager = std::move(aOther.myEntityManager);
		myComponentManager = std::move(aOther.myComponentManager);
		mySystemManager = std::move(aOther.mySystemManager);

		myEntityManager.myComponentManager = &myComponentManager;

		return *this;
	}

	void EntityComponentSystem::Init()
	{
		mySystemManager.Init();
	}

	void EntityComponentSystem::Update()
	{
		mySystemManager.Update(this);
	}

	void EntityComponentSystem::Render()
	{
		mySystemManager.Render(this);
	}

	void EntityComponentSystem::EarlyUpdate()
	{
		mySystemManager.EarlyUpdate(this);
	}

	void EntityComponentSystem::FixedUpdate()
	{
		mySystemManager.FixedUpdate(this);
	}

	void EntityComponentSystem::LateUpdate()
	{
		mySystemManager.LateUpdate(this);
	}

	void EntityComponentSystem::LateRender()
	{
		mySystemManager.LateRender(this);
	}

	void EntityComponentSystem::AddClonedSystem(const size_t aSystemHashCode, std::unique_ptr<System> aSystem)
	{
		mySystemManager.mySystems[aSystemHashCode] = std::move(aSystem);
	}

	ECS::IEntity& EntityComponentSystem::CreateEntity(const EntityID aEntityID)
	{
		return myEntityManager.CreateEntity(aEntityID);
	}

	bool EntityComponentSystem::RemoveEntity(const EntityID aEntityID)
	{
		return myEntityManager.DestroyEntity(aEntityID);
	}

	ECS::IEntity& EntityComponentSystem::GetEntity(const EntityID aID)
	{
		return myEntityManager.GetEntity(aID);
	}

	std::vector<ECS::IEntity>& EntityComponentSystem::GetAllEntities()
	{
		return myEntityManager.GetAllEntities();
	}

	void* EntityComponentSystem::GetComponentPointerByComponentID(const ComponentID aComponentID)
	{
		return myComponentManager.GetComponentByComponentID(aComponentID);
	}

	void EntityComponentSystem::SaveData(EntityComponentSystem& aECS, const std::string& aFileName)
	{
		const std::vector<IEntity>& entities = aECS.GetAllEntities();

		nlohmann::ordered_json jsonData;

		for (size_t i = 0; i < entities.size(); ++i)
		{
			const ECS::IEntity& entity = entities[i];
			const std::unordered_map<ECS::ComponentType, ComponentID>& components = entity.GetComponentMap();

			jsonData["Entities"][i]["ID"] = entity.GetID();;
			jsonData["Entities"][i]["Name"] = entity.GetName();;

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
		const std::string absoluteFilePath = SimpleUtilities::GetAbsolutePath(aFileName);
		const nlohmann::json jsonData = SimpleUtilities::FileManager::GetDataAsJson(absoluteFilePath);

		if (jsonData.contains("Entities") == false || jsonData.is_null() == true)
		{
			return;
		}

		const ComponentRegistry* componentRegistry = MainSingleton::GetComponentRegistry();
		bool shouldUpdateJSON = false;

		for (size_t i = 0; i < jsonData["Entities"].size(); ++i)
		{
			const nlohmann::json& entityData = jsonData["Entities"][i];
			const EntityID id = entityData["ID"];
			const std::string name = entityData["Name"];

			ECS::IEntity& entity = aECS.CreateEntity(id);
			entity.SetName(name);

			if (entityData.contains("Components") == false)
			{
				continue;
			}

			for (size_t j = 0; j < jsonData["Entities"][i]["Components"].size(); ++j)
			{
				const nlohmann::json& componentDataJSON = entityData["Components"][j];

				if (componentRegistry->myComponentNameToHashCode.contains(componentDataJSON["Name"]) == false)
				{
					EraseMissingElementFromJSON(jsonData, absoluteFilePath, i, j); //NOTE(v11.3.2): Maybe not needed but works for now
					continue;
				}

				const size_t componentHashCode = componentRegistry->myComponentNameToHashCode.at(componentDataJSON["Name"]);
				const std::vector<ComponentProperty>& componentProperties = componentRegistry->myTypeErasureComponents.at(componentHashCode).myComponentProperties;
				const ComponentID componentID = componentRegistry->myTypeErasureComponents.at(componentHashCode).AddComponentFunctionPointer(entity);

				if (componentDataJSON.contains("Properties") == false)
				{
					continue;
				}

				nlohmann::json componentPropertiesJSON = componentDataJSON["Properties"];

				void* componentPointer = aECS.GetComponentPointerByComponentID(componentID);

				if (SimpleUtilities::FileManager::IsJSONDataDifferentFromPropertyData(componentProperties, componentPropertiesJSON) == true)
				{
					shouldUpdateJSON = true;
				}

				LoadComponentData(componentPropertiesJSON, componentProperties, componentRegistry, componentPointer);
			}
		}

		if (shouldUpdateJSON)
		{
			SaveData(aECS, aFileName);
			Simple::Console::Print(SimpleUtilities::ConvertFilePathToPrettyName(aFileName).c_str(), Simple::ConsoleTextColor::Red, false);
			Simple::Console::Print(" has been updated due to changes in the source code", Simple::ConsoleTextColor::White, true);
		}
	}

	void EntityComponentSystem::EraseMissingElementFromJSON(const nlohmann::json& aJsonData, const std::string& aAbsolutePath, const size_t aEntityIndex, const size_t aComponentIndex)
	{
		nlohmann::json newData = aJsonData;
		auto& entityJson = newData["Entities"][aEntityIndex];

		nlohmann::json entityComponentJson = entityJson;
		entityComponentJson["Components"].erase(aComponentIndex);

		entityJson = entityComponentJson;
		newData["Entities"][aEntityIndex] = entityJson;

		std::ofstream writeFile(aAbsolutePath);
		assert(writeFile.is_open() && "Failed to open the file");

		writeFile << newData;
		writeFile.close();
	}

	void EntityComponentSystem::LoadComponentData(nlohmann::json& aPropertiesJSON, const std::vector<ComponentProperty>& aComponentProperties, const ComponentRegistry* aComponentRegistry, void* aComponentPointer)
	{
		for (auto it = aPropertiesJSON.begin(); it != aPropertiesJSON.end(); )
		{
			bool found = false;

			for (const auto& prop : aComponentProperties)
			{
				if (it.key() == prop.name)
				{
					const TypeErasureObject& typeErasedData = aComponentRegistry->myTypeErasureDataTypes.at(prop.id);
					void* propertyPointer = reinterpret_cast<void*>((reinterpret_cast<size_t>(aComponentPointer) + prop.byteOffset));

					if (const auto& loadDataFromJSONFunction = typeErasedData.LoadDataFromJSON)
					{
						loadDataFromJSONFunction(propertyPointer, prop.name, aPropertiesJSON);
					}

					found = true;
					break;
				}
			}

			if (found)
			{
				it = aPropertiesJSON.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}