#include "Game/Precomplied/GamePch.hpp"
#include "Game/GameWorld.hpp"
#include "Game/Managers/LevelManager/LevelManager.hpp"
#include "Game/Managers/RaycastManager/RaycastManager.hpp"
#include "Game/Managers/EventManager/EventManager.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace Simple
{
	GameWorld::GameWorld()
		: myLevelManager(std::make_unique<Simple::LevelManager>())
		, myRaycastManager(std::make_unique<Simple::RaycastManager>())
		, myEventManager(std::make_unique<Simple::EventManager>())
		, myFixedUpdateTimer(0.0f)
		, myFixedUpdateFreqency(1.0f / 60.0f)
	{
	}

	GameWorld::~GameWorld()
	{
	}

	void GameWorld::Init()
	{
		myLevelManager->Init();
		myRaycastManager->Init();

		MainSingleton::GetSceneManager().Init();
	}

	void GameWorld::Update()
	{
		EarlyUpdate();

		FixedUpdate();

		NormalUpdate();

		LateUpdate();
	}

	void GameWorld::Render()
	{
		myLevelManager->Render();
		myRaycastManager->Render();

		MainSingleton::GetSceneManager().Render();
	}

	void GameWorld::LateRender()
	{
		MainSingleton::GetSceneManager().LateRender();
	}

	void GameWorld::NormalUpdate()
	{
		myLevelManager->Update();
		myRaycastManager->Update();

		MainSingleton::GetSceneManager().Update();
	}

	void GameWorld::EarlyUpdate()
	{
	}

	void GameWorld::FixedUpdate()
	{
		myFixedUpdateTimer += Global::GetDeltaTime();

		if (myFixedUpdateTimer < myFixedUpdateFreqency)
		{
			return;
		}

		myFixedUpdateTimer = 0.0f;
	}

	void GameWorld::LateUpdate()
	{
	}
}