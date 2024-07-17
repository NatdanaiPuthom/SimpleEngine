#include "MainSingleton/SceneManager/SceneManager.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/Global.hpp"
#include "External/profiler.h"
#include "External/nlohmann/json.hpp"
#include "MainSingleton/MainSingleton.hpp"
#include "Game/Systems/AddSystemHere/AllGameSystems.hpp"
#include <fstream>

namespace Simpleton
{
	SceneManager::SceneManager()
		: myNextSceneID(0)
		, myCurrentSceneInfo(nullptr)
		, myIsPlaying(false)
	{
#ifdef _SIMPLE
		myIsPlaying = true;
#endif
	}

	SceneManager::~SceneManager()
	{
	}

	void SceneManager::Destroy()
	{
		myECSs.clear();
		myCurrentSceneInfo = nullptr;
	}

	void SceneManager::Init()
	{
		LoadSystems();
		LoadSettingsFromJson();

		auto camera = Global::GetGraphicsEngine()->GetEditorCamera();
		camera->SetRotation(Math::Vector3f(30, 0, 0));
		camera->SetPosition(Math::Vector3f(1, 9, -12));
	}

	void SceneManager::Update()
	{
		if (myIsPlaying)
		{
			myECSs[myCurrentSceneInfo->id].Update();
		}
	}

	void SceneManager::Render()
	{
		myECSs[myCurrentSceneInfo->id].Render();
	}

	void SceneManager::EarlyUpdate()
	{
		if (myIsPlaying)
		{
			myECSs[myCurrentSceneInfo->id].EarlyUpdate();
		}
	}

	void SceneManager::FixedUpdate()
	{
		if (myIsPlaying)
		{
			myECSs[myCurrentSceneInfo->id].FixedUpdate();
		}
	}

	void SceneManager::LateUpdate()
	{
		if (myIsPlaying)
		{
			myECSs[myCurrentSceneInfo->id].LateUpdate();
		}
	}

	void SceneManager::LateRender()
	{
		myECSs[myCurrentSceneInfo->id].LateRender();
	}

	void SceneManager::ChangeScene(const std::string& aSceneName)
	{
		if (mySceneInfos.contains(aSceneName) == false)
		{
			AddScene(aSceneName);
		}

		myCurrentSceneInfo = &mySceneInfos[aSceneName];
	}

	void SceneManager::ChangeSceneName(const std::string& aNewSceneName)
	{
		Simpleton::SceneInfo newSceneInfo;

		newSceneInfo.id = myCurrentSceneInfo->id;
		newSceneInfo.name = aNewSceneName;
		newSceneInfo.relativePath = std::string(SIMPLE_DIR_SCENES) + "\\" + std::string(newSceneInfo.name).c_str() + std::string(".scene").c_str();
		newSceneInfo.absolutePath = SimpleUtilities::GetAbsolutePath(newSceneInfo.relativePath);

		if (std::rename(myCurrentSceneInfo->absolutePath.c_str(), newSceneInfo.absolutePath.c_str()) != 0)
		{
			assert(false && "Failed to rename the scene");
			return;
		}

		mySceneInfos.erase(myCurrentSceneInfo->relativePath);
		mySceneInfos[newSceneInfo.relativePath] = newSceneInfo;

		myCurrentSceneInfo = &mySceneInfos[newSceneInfo.relativePath];
	}

	void SceneManager::CreateNewScene(const std::string& aFilePath)
	{
		std::ofstream writeFile(aFilePath);
		assert(writeFile.is_open() && "Failed to open the file");

		nlohmann::json json;
		writeFile << json.dump(-1);
		writeFile.close();
	}

	void SceneManager::ReloadSceneFromFile(const std::string& aSceneName)
	{
		if (mySceneInfos.contains(aSceneName))
		{
			myECSs.erase(mySceneInfos[aSceneName].id);
			LoadAndInitScene(aSceneName);
		}
	}

	const SceneInfo* SceneManager::GetCurrentSceneInfo() const
	{
		return myCurrentSceneInfo;
	}

	ECS::EntityComponentSystem& SceneManager::GetCurrentECS()
	{
		return myECSs[myCurrentSceneInfo->id];
	}

	void SceneManager::LoadSettingsFromJson()
	{
		const std::string filename = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_GAME);

		std::ifstream file(filename);
		assert(file.is_open() && "Failed To Open File");

		const nlohmann::json jsonData = nlohmann::json::parse(file);
		const nlohmann::json gameSettings = jsonData["Game_Settings"];
		file.close();

		const std::string sceneFilePath = SimpleUtilities::GetAbsolutePath(gameSettings["Start_Scene_RelativePath"]);
		std::ifstream sceneFile(sceneFilePath);

		if (sceneFile.is_open() == false)
		{
			std::ofstream writeFile(sceneFilePath);
			assert(writeFile.is_open() && "Failed to create the file");

			const nlohmann::json emptyJson = {};
			writeFile << emptyJson.dump(-1);
			writeFile.close();
		}

		LoadDefaultScene(gameSettings["Start_Scene_RelativePath"]);
	}

	void SceneManager::LoadSystems()
	{
		ECS::IECSGameSystem::AddSystems();
	}

	void SceneManager::LoadDefaultScene(const std::string& aDefaultScenePath)
	{
		ChangeScene(aDefaultScenePath);
	}

	bool SceneManager::LoadAndInitScene(const std::string& aSceneName)
	{
		const bool success = myECSs.try_emplace(mySceneInfos[aSceneName].id).second;

		if (success)
		{
			ECS::EntityComponentSystem& ecs = myECSs[mySceneInfos[aSceneName].id];

			for (const auto& [hashCode, system] : ECS::ECSGameSystem::mySystems)
			{
				ecs.AddClonedSystem(hashCode, system->Clone(&ecs));
			}

			ecs.Init();

			const std::string name = "ECS LoadScene: " + mySceneInfos[aSceneName].name;

			PROFILER_BEGIN(name);
			ECS::EntityComponentSystem::LoadData(ecs, mySceneInfos[aSceneName].relativePath);
			PROFILER_END();
		}
		else
		{
			assert(false && "Failed to AddScene");
		}

		return success;
	}

	bool SceneManager::AddScene(const std::string& aSceneName)
	{
		SceneInfo sceneInfo;

		sceneInfo.id = myNextSceneID++;
		sceneInfo.name = SimpleUtilities::ConvertFilePathToPrettyName(aSceneName, false);
		sceneInfo.relativePath = aSceneName;
		sceneInfo.absolutePath = SimpleUtilities::GetAbsolutePath(aSceneName);

		mySceneInfos[aSceneName] = sceneInfo;

		return LoadAndInitScene(aSceneName);
	}

	void SceneManager::SetIsPlaying(const bool aIsPlaying)
	{
		myIsPlaying = aIsPlaying;
	}

	bool SceneManager::GetIsPlaying() const
	{
		return myIsPlaying;
	}
}