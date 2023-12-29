#include "Game/Precomplier/stdafx.h"
#include "Engine/Graphics/Camera/Camera.hpp"
#include "Game/Managers/LevelManager/LevelManager.hpp"
#include "Game/Managers/LevelManager/Scenes/DefaultScene.hpp"
#include "Game/Managers/LevelManager/Scenes/Spotlights.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"

LevelManager::LevelManager()
{
	SimpleWorldLevelManagerImpl::SetLevelManager(this);
}

LevelManager::~LevelManager()
{
}

void LevelManager::Init()
{
	auto camera = SimpleGlobal::GetGraphicsEngine()->GetCamera();
	camera->SetRotation(SimpleUtilities::Vector3f(50, 0, 0));
	camera->SetPosition(SimpleUtilities::Vector3f(10, 15, -12));

	std::shared_ptr<DefaultScene> defaultScene = std::make_shared<DefaultScene>();
	defaultScene->Init();
	myScenes.emplace(0, defaultScene);

	std::shared_ptr<SpotlightScene> spotlightScene = std::make_shared<SpotlightScene>();
	spotlightScene->Init();
	myScenes.emplace(1, spotlightScene);

	SimpleWorld::SetActiveScene(SimpleWorld::GetActiveSceneIndex());
}

void LevelManager::Update()
{
	myActiveScene->Update();
	myActiveScene->myDirectionalLight->Update();
}

void LevelManager::Render()
{
	Renderer* renderer = SimpleGlobal::GetRenderer();

	for (const auto& model : myActiveScene->myModelInstances)
	{
		renderer->RenderModel(model);
	}

	if (renderer->IsDebugModeOn())
	{
		for (const auto& model : myActiveScene->myModelInstances)
		{
			renderer->RenderBoundingBox(model);
		}
	}

	myActiveScene->myDirectionalLight->Render();
}

void LevelManager::SetActiveScene(const int aSceneIndex)
{
	myActiveScene = myScenes.at(aSceneIndex);
}

std::shared_ptr<Scene> LevelManager::GetActiveScene()
{
	return myActiveScene;
}
