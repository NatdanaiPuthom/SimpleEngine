#include "Game/Precomplied/GamePch.hpp"
#include "Game/GameWorld.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"
#include "Game/Managers/LevelManager/LevelManager.hpp"
#include "Game/Managers/RaycastManager/RaycastManager.hpp"
#include "Game/Managers/EventManager/EventManager.hpp"
#include "Engine/Math/Quaternion.hpp"

namespace Simple
{
	GameWorld::GameWorld()
		: myLevelManager(std::make_unique<Simple::LevelManager>())
		, myImGuiManager(std::make_unique<Simple::ImGuiManager>())
		, myRaycastManager(std::make_unique<Simple::RaycastManager>())
		, myEventManager(std::make_unique<Simple::EventManager>())
		, myFixedUpdateTimer(0.0f)
		, myFixedUpdateFreqency(1.0f / 60.0f)
	{
		Impl::SimpleWorldGameWorld::SetGameWorld(this);
	}

	GameWorld::~GameWorld()
	{
	}

	void GameWorld::Init()
	{
		myImGuiManager->Init();
		myLevelManager->Init();
		myRaycastManager->Init();

		Math::Quaternion quaternion;

		std::cout << quaternion.GetRight();
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
		Global::GetGraphicsEngine()->SetRenderTarget(eRenderTarget::WaterReflection);
		Global::GetGraphicsEngine()->GetRenderer()->RenderEverythingUpSideDown();

		Global::GetGraphicsEngine()->SetRenderTarget(eRenderTarget::WaterRefraction);
		Global::GetGraphicsEngine()->GetRenderer()->RenderRefraction();

		Global::GetGraphicsEngine()->SetRenderTarget(eRenderTarget::Backbuffer);

		myLevelManager->Render();
		myRaycastManager->Render();

		myImGuiManager->Render();
	}

	void GameWorld::NormalUpdate()
	{
		myLevelManager->Update();
		myRaycastManager->Update();
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