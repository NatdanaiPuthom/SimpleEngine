#include "Game/Precomplier/stdafx.h"
#include "Engine/Graphics/Model/PlaneReflection.h"
#include "Engine/Graphics/Renderer/LineDrawer.hpp"
#include "Game/GameWorld.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"
#include "Game/Managers/LevelManager/LevelManager.hpp"


GameWorld::GameWorld()
	: myImGuiManager(std::make_unique<ImGuiManager>())
	, myLevelManager(std::make_unique<LevelManager>())
	, myLine(std::make_unique<Drawer::Line>())
{
	SimpleWorldGameWorldImpl::SetGameWorld(this);

	myPlaneReflection = std::make_unique<PlaneReflection>();

	myLine->color = { 1,0,0, 1 };
	myLine->startPosition = { 1,0,-1 };
	myLine->endPosition = { 10,5,-1 };
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
	SimpleGlobal::GetRenderer()->RenderLine(*myLine);
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

