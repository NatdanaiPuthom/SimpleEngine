#include "Game/Precomplier/stdafx.h"
#include "Engine/Graphics/Camera/Camera.hpp"
#include "Game/Managers/LevelManager/LevelManager.hpp"
#include "Game/Managers/LevelManager/Scenes/DefaultScene.hpp"
#include "Game/Managers/LevelManager/Scenes/SpotlightScene.hpp"
#include "Game/Managers/LevelManager/Scenes/NavmeshSceneTest.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"

namespace Simple
{
	LevelManager::LevelManager()
		: myActiveScene(0)
	{
		LoadSettingsFromJson();

		Impl::SimpleWorldLevelManager::SetLevelManager(this);
	}

	LevelManager::~LevelManager()
	{
	}

	void LevelManager::Init()
	{
		auto camera = SimpleGlobal::GetGraphicsEngine()->GetCamera();
		camera->SetRotation(SimpleUtilities::Vector3f(50, 0, 0));
		camera->SetPosition(SimpleUtilities::Vector3f(10, 15, -12));

		std::shared_ptr<Scenes::DefaultScene> defaultScene = std::make_shared<Scenes::DefaultScene>();
		defaultScene->Init();
		myScenes.emplace(0, defaultScene);

		std::shared_ptr<Scenes::SpotlightScene> spotlightScene = std::make_shared<Scenes::SpotlightScene>();
		spotlightScene->Init();
		myScenes.emplace(1, spotlightScene);

		std::shared_ptr<Scenes::NavmeshSceneTest> navmeshTest = std::make_shared<Scenes::NavmeshSceneTest>();
		navmeshTest->Init();
		myScenes.emplace(2, navmeshTest);

		SetActiveScene(myActiveSceneIndex);

		myNavmesh.Init("Intro To Boss.obj");
	}

	void LevelManager::Update()
	{
		myActiveScene->Update();
	}

	void LevelManager::Render()
	{
		myActiveScene->Render();
	}

	void LevelManager::SetActiveScene(const int aSceneIndex)
	{
		myActiveSceneIndex = aSceneIndex;
		myActiveScene = myScenes.at(myActiveSceneIndex);
	}

	Simple::Navmesh& LevelManager::GetNavmesh()
	{
		return myNavmesh;
	}

	std::shared_ptr<Simple::Scene> LevelManager::GetActiveScene()
	{
		return myActiveScene;
	}

	int LevelManager::GetActiveSceneIndex() const
	{
		return myActiveSceneIndex;
	}

	void LevelManager::LoadSettingsFromJson()
	{
		const std::string filename = SimpleUtilities::GetPath(SIMPLE_LEVELS_FILENAME);
		std::ifstream file(filename);
		assert(file.is_open() && "Failed To Open File");

		const nlohmann::json json = nlohmann::json::parse(file);
		file.close();

		myActiveSceneIndex = json["activeScene"];
	}
}