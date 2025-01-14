#include "Game/Precomplied/GamePch.hpp"
#include "Game/GameWorld.hpp"
#include "Game/ECS/Components/AllGameComponents.hpp"
#include "Game/ECS/Systems/AllGameSystems.hpp"
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

	void GameWorld::EarlyUpdate()
	{
		MainSingleton::GetSceneManager().EarlyUpdate();
	}

	void GameWorld::FixedUpdate()
	{
		myFixedUpdateTimer += Global::GetDeltaTime();

		if (myFixedUpdateTimer < myFixedUpdateFreqency)
		{
			return;
		}

		myFixedUpdateTimer = 0.0f;

		MainSingleton::GetSceneManager().FixedUpdate();
	}

	void GameWorld::Update()
	{
		MainSingleton::GetSceneManager().Update();
	}

	void GameWorld::LateUpdate()
	{
		MainSingleton::GetSceneManager().LateUpdate();
	}

	void GameWorld::Render()
	{
		MainSingleton::GetSceneManager().Render();
	}

	void GameWorld::LateRender()
	{
		MainSingleton::GetSceneManager().LateRender();
	}

	void GameWorld::RenderBloomEntities()
	{
		MainSingleton::GetSceneManager().RenderBloomEntities();
	}
}