#include "Game/Precomplier/stdafx.h"
#include "Engine/Graphics/Camera/Camera.hpp"
#include "Game/Managers/LevelManager/LevelManager.hpp"
#include "Game/Managers/LevelManager/Scenes/Default.hpp"
#include "Game/Managers/LevelManager/Scenes/Spotlights.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"

namespace Simple
{
	LevelManager::LevelManager()
	{
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

		std::shared_ptr<Scenes::Default> defaultScene = std::make_shared<Scenes::Default>();
		defaultScene->Init();
		myScenes.emplace(0, defaultScene);

		std::shared_ptr<Scenes::Spotlight> spotlightScene = std::make_shared<Scenes::Spotlight>();
		spotlightScene->Init();
		myScenes.emplace(1, spotlightScene);

		SimpleWorld::SetActiveScene(SimpleWorld::GetActiveSceneIndex());
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
		myActiveScene = myScenes.at(aSceneIndex);
	}

	std::shared_ptr<Simple::Scene> LevelManager::GetActiveScene()
	{
		return myActiveScene;
	}
}