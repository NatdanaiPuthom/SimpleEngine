#include "Game/Precomplier/stdafx.h"
#include "Engine/Graphics/Model/PlaneReflection.h"
#include "Game/GameWorld.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"
#include "Game/Managers/LevelManager/LevelManager.hpp"


GameWorld::GameWorld()
	: myImGuiManager(std::make_unique<ImGuiManager>())
	, myLevelManager(std::make_unique<LevelManager>())
{
	SimpleWorldGameWorldImpl::SetGameWorld(this);

	myPlaneReflection = std::make_unique<PlaneReflection>();
}

GameWorld::~GameWorld()
{
}

void GameWorld::Init()
{
	myLevelManager->Init();
}

void GameWorld::Update()
{
	/*while (SimpleGlobal::GetGameIsRunning())
	{
	}*/

	myLevelManager->Update();
}

void GameWorld::Render()
{
	myLevelManager->Render();
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

