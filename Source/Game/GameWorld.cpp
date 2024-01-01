#include "Game/Precomplier/stdafx.h"
#include "Engine/Graphics/Model/Special/PlaneReflection.h"
#include "Game/GameWorld.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"
#include "Game/Managers/LevelManager/LevelManager.hpp"
#include "Game/Managers/RaycastManager/RaycastManager.hpp"

namespace Simple
{
	GameWorld::GameWorld()
		: myImGuiManager(std::make_unique<Simple::ImGuiManager>())
		, myLevelManager(std::make_unique<Simple::LevelManager>())
		, myRaycastManager(std::make_unique<Simple::RaycastManager>())
	{
		Impl::SimpleWorldGameWorld::SetGameWorld(this);

		myPlaneReflection = std::make_unique<Simple::PlaneReflection>();
		SimpleWorld::SetWaterPlane(myPlaneReflection->myModel);
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
		/*
		while (SimpleGlobal::GetGameIsRunning())
		{
		}
		*/

		myPlaneReflection->Update();
		myLevelManager->Update();
		myRaycastManager->Update();
	}

	void GameWorld::Render()
	{
		myLevelManager->Render();
		myRaycastManager->Render();
	}

	void GameWorld::RenderImGui()
	{
		myImGuiManager->Render();
	}

	void GameWorld::RenderReflection()
	{
		SimpleGlobal::GetRenderer()->RenderPlaneReflection(myPlaneReflection->myModel.get());
	}

	void GameWorld::RenderEverythingUpSideDown()
	{
		SimpleGlobal::GetRenderer()->RenderEverythingUpSideDown();
	}
}