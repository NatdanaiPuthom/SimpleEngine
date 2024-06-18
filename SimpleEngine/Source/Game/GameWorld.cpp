#include "Game/Precomplied/GamePch.hpp"
#include "Game/GameWorld.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace Simple
{
	GameWorld::GameWorld()
		: myFixedUpdateTimer(0.0f)
		, myFixedUpdateFreqency(1.0f / 60.0f)
	{
	}

	GameWorld::~GameWorld()
	{
	}

	void GameWorld::Init()
	{
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
		MainSingleton::GetSceneManager().Render();
	}

	void GameWorld::LateRender()
	{
		MainSingleton::GetSceneManager().LateRender();
	}

	void GameWorld::NormalUpdate()
	{
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