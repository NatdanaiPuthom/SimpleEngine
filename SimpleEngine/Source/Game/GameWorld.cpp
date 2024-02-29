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

		myAnimationTest.animatedModel = Global::GetModelFactory()->LoadAnimatedModel("Assets/Models/SM_wizard.fbx");
		myAnimationTest.animation = Global::GetModelFactory()->LoadAnimationFBX("Assets/Models/A_Wizard_Falling.fbx");

		myAnimationTest.animatedModel.SetShader("DefaultPS.cso", "AnimatedModelVS.cso");
		myAnimationTest.animatedModel.SetScale(0.01f);
		myAnimationTest.animatedModel.ClearTextures();
		myAnimationTest.animatedModel.AddTexture("SM_Wizard_c.dds");

		myAnimationTest.animationPlayer.Init(myAnimationTest.animation, myAnimationTest.animatedModel);
		myAnimationTest.animationPlayer.SetIsLooping(true);
		myAnimationTest.animationPlayer.Play();
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

		myAnimationTest.animationPlayer.Update();
		myAnimationTest.animatedModel.SetPose(myAnimationTest.animationPlayer.myLocalSpacePose);

		Global::GetRenderer()->RenderAnimatedSkeletonLines(myAnimationTest.animatedModel, myAnimationTest.animationPlayer.myLocalSpacePose);
		Global::GetRenderer()->RenderModel(myAnimationTest.animatedModel);
		Global::GetRenderer()->RenderBoundingBox(myAnimationTest.animatedModel);

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