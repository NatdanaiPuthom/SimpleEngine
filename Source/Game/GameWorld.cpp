#include "Game/Precomplier/stdafx.h"
#include "Game/GameWorld.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"
#include "Game/Managers/LevelManager/LevelManager.hpp"
#include "Game/Managers/RaycastManager/RaycastManager.hpp"
#include "Game/Managers/EventManager/EventManager.hpp"

namespace Simple
{
	GameWorld::GameWorld()
		: myLevelManager(std::make_unique<Simple::LevelManager>())
		, myImGuiManager(std::make_unique<Simple::ImGuiManager>())
		, myRaycastManager(std::make_unique<Simple::RaycastManager>())
		, myEventManager(std::make_unique<Simple::EventManager>())
	{
		Impl::SimpleWorldGameWorld::SetGameWorld(this);
	}

	GameWorld::~GameWorld()
	{
	}

	void GameWorld::Init()
	{
		myLevelManager->Init();
		myRaycastManager->Init();
	}

	void GameWorld::Update()
	{
		/*
		while (SimpleGlobal::GetGameIsRunning())
		{
		}
		*/

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
}