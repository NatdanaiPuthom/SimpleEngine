#include "MainSingleton/SceneManager/SceneManager.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/Global.hpp"
#include "External/profiler.h"
#include "External/nlohmann/json.hpp"
#include <fstream>
#include "MainSingleton.hpp"

namespace Simpleton
{
	SceneManager::SceneManager()
	{
	}

	SceneManager::~SceneManager()
	{
	}

	void SceneManager::Destroy()
	{
		myECSs.clear();
	}

	void SceneManager::Init()
	{
		LoadSettingsFromJson();
		LoadDefaultScene();

		auto camera = Global::GetGraphicsEngine()->GetEditorCamera();
		camera->SetRotation(Math::Vector3f(30, 0, 0));
		camera->SetPosition(Math::Vector3f(1, 9, -12));
	}

	void SceneManager::Update()
	{
		myECSs[myCurrentScene].Update();

		if (MainSingleton::GetInputManager().IsKeyPressed(VK_RETURN))
		{
			ChangeScene("Assets\\Scenes\\Test_Scene.scene");
		}
	}

	void SceneManager::Render()
	{
		myECSs[myCurrentScene].Render();
	}

	void SceneManager::LateRender()
	{
		myECSs[myCurrentScene].RenderPointLights();
		myECSs[myCurrentScene].RenderSkyBoxAndDirectionalLight();
	}

	void SceneManager::ChangeScene(const std::string& aSceneName)
	{
		if (myECSs.contains(aSceneName) == false)
		{
			AddScene(aSceneName);
		}

		myCurrentScene = aSceneName;
	}

	const std::string& SceneManager::GetCurrentScenePath() const
	{
		return myCurrentScene;
	}

	ECS::EntityComponentSystem& SceneManager::GetCurrentECS()
	{
		return myECSs[myCurrentScene];
	}

	void SceneManager::LoadSettingsFromJson()
	{
		const std::string filename = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_GAME);

		std::ifstream file(filename);
		assert(file.is_open() && "Failed To Open File");

		const nlohmann::json jsonData = nlohmann::json::parse(file);
		const nlohmann::json gameSettings = jsonData["Game_Settings"];
		file.close();

		const std::string levelFileName = SimpleUtilities::GetAbsolutePath(gameSettings["Start_Scene"]);
		std::ifstream levelJsonFile(levelFileName);

		if (levelJsonFile.is_open() == false)
		{
			std::ofstream writeFile(levelFileName);
			assert(writeFile.is_open() && "Failed to create the file");

			const nlohmann::json emptyJson = {};
			writeFile << emptyJson.dump(-1);
			writeFile.close();
		}

		myCurrentScene = gameSettings["Start_Scene"];
	}

	void SceneManager::LoadDefaultScene()
	{
		myECSs.try_emplace(myCurrentScene);

		myECSs[myCurrentScene].Init();

		PROFILER_BEGIN("ECS Load DefaultScene");
		ECS::EntityComponentSystem::LoadData(myECSs[myCurrentScene], myCurrentScene);
		PROFILER_END();
	}

	bool SceneManager::AddScene(const std::string& aSceneName)
	{
		const bool success = myECSs.try_emplace(aSceneName).second;

		if (success)
		{
			myECSs[aSceneName].Init();

			const std::string name = "ECS Load " + aSceneName;

			PROFILER_BEGIN(name);
			ECS::EntityComponentSystem::LoadData(myECSs[aSceneName], aSceneName);
			PROFILER_END();
		}

		return success;
	}
}