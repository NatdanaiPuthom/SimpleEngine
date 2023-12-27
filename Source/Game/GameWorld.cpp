#include "Game/Precomplier/stdafx.h"
#include "Engine/Graphics/Camera/Camera.hpp"
#include "Engine/Graphics/Model/PlaneReflection.h"
#include "Engine/Graphics/Renderer/BoundingBoxDrawer.hpp"
#include "Game/GameWorld.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"
#include "Game/Managers/LevelManager/DefaultScene.hpp"

GameWorld::GameWorld()
	: myImGuiManager(std::make_unique<ImGuiManager>())
{
	SimpleWorldGameWorldImpl::SetGameWorld(this);

	myPlaneReflection = std::make_unique<PlaneReflection>();

	{
		std::shared_ptr<DefaultScene> defaultScene = std::make_shared<DefaultScene>();
		defaultScene->Init();

		myScenes.emplace(0, defaultScene);
	}

	SimpleWorld::SetActiveScene(0);
}

GameWorld::~GameWorld()
{
}

void GameWorld::Init()
{
	{
		auto camera = SimpleGlobal::GetGraphicsEngine()->GetCamera();
		camera->SetRotation(SimpleUtilities::Vector3f(50, 0, 0));
		camera->SetPosition(SimpleUtilities::Vector3f(10, 15, -12));
	}
}

void GameWorld::Update()
{
	/*while (SimpleGlobal::GetGameIsRunning())
	{
	}*/

	myActiveScene->Update();
}

void GameWorld::Render()
{
	Renderer* renderer = SimpleGlobal::GetRenderer();

	for (const auto& model : myActiveScene->myModelInstances)
	{
		renderer->Render(model);
	}

	if (renderer->IsDebugModeOn())
	{
		for (const auto& model : myActiveScene->myModelInstances)
		{
			renderer->RenderBoundingBox(model);
		}
	}
}

void GameWorld::RenderImGui()
{
	myImGuiManager->Render();
}

void GameWorld::RenderReflection()
{
	//SimpleGlobal::GetRenderer()->RenderPlaneReflection(myPlaneReflection->myModelInstance.get());
}

void GameWorld::RenderUpSideDown()
{
	/*Renderer* renderer = SimpleGlobal::GetRenderer();
	for (const auto& model : myModelInstances)
	{
		renderer->RenderEverythingUpSideDown(model.get());
	}*/
}

void GameWorld::SetActiveScene(const int aSceneIndex)
{
	myActiveScene = myScenes.at(aSceneIndex);
}

std::shared_ptr<Scene> GameWorld::GetActiveScene()
{
	return myActiveScene;
}
