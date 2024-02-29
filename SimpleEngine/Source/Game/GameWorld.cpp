#include "Game/Precomplied/GamePch.hpp"
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

		NatdanaiAnimationTest animationTest;
		animationTest.animatedModel = Global::GetModelFactory()->LoadAnimatedModel("Assets/Models/SM_wizard.fbx");
		animationTest.animation = Global::GetModelFactory()->LoadAnimationFBX("Assets/Models/A_Wizard_Falling.fbx");

		animationTest.animatedModel.SetShader("DefaultPS.cso", "AnimatedModelVS.cso");
		animationTest.animatedModel.SetScale(0.01f);
		animationTest.animatedModel.ClearTextures();
		animationTest.animatedModel.AddTexture("SM_Wizard_c.dds");

		animationTest.animationPlayer.Init(animationTest.animation, animationTest.animatedModel);
		animationTest.animationPlayer.SetIsLooping(true);
		animationTest.animationPlayer.Play();

		for (size_t i = 0; i < 200; ++i)
		{
			myThreadTest.push_back(animationTest);
		}

		const float spacing = 5.0f;
		const size_t numRows = static_cast<size_t>(std::sqrt(myThreadTest.size()));
		const size_t numCols = numRows;

		for (size_t i = 0; i < myThreadTest.size(); ++i)
		{
			const float x = spacing * static_cast<float>((i % numCols));
			const float z = spacing * static_cast<float>((i / numRows)); 
			const Math::Vector3f pos(x, 0.0f, z);
			myThreadTest[i].animatedModel.SetPosition(pos);
		}

		myFutureResults.resize(myThreadTest.size());
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

		for (size_t i = 0; i < myThreadTest.size(); ++i)
		{
			std::future<bool> result = myThreadPool.AddTask(&AnimationPlayer::UpdateThreaded, std::ref(myThreadTest[i].animationPlayer), std::ref(myThreadTest[i].animatedModel), std::ref(myThreadTest[i].animation));
			myFutureResults[i] = std::move(result);
		}

		for (auto& future : myFutureResults)
		{
			future.get();
		}

		for (size_t i = 0; i < myThreadTest.size(); ++i)
		{
			Global::GetRenderer()->RenderModel(myThreadTest[i].animatedModel);
		}

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