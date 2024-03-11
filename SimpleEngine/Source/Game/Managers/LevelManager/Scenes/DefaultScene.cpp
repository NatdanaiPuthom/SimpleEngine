#include "Game/Precomplied/GamePch.hpp"
#include "Game/Managers/LevelManager/Scenes/DefaultScene.hpp"

namespace Scenes
{
	Scenes::DefaultScene::DefaultScene()
	{
	}

	Scenes::DefaultScene::~DefaultScene()
	{
	}

	void DefaultScene::OnEnter()
	{
		auto camera = Global::GetGraphicsEngine()->GetDefaultCamera();
		camera->SetRotation(Math::Vector3f(50, 0, 0));
		camera->SetPosition(Math::Vector3f(10, 15, -12));

		myDirectionalLight->myModel->SetPosition({ 0.0f, 10.0f, 1.0f });
		Global::GetGraphicsEngine()->SetDirectionalLightDirection({ 0.0f, -1.0f, 0.0f });
	}

	void Scenes::DefaultScene::Init()
	{
		myFloor = Global::GetModelFactory()->LoadStaticModelFBX("StaticModels/Simple_Floor_10x10.fbx");

		myPlayer.myModel = Global::GetModelFactory()->LoadAnimatedModelFBX("AnimatedModels/SM_Wizard.fbx");
		myPlayer.myModel.SetScale(0.01f);
		myPlayer.myModel.ClearTextures();
		myPlayer.myModel.AddTexture("Models/SM_Wizard_c.dds");

		myPlayer.myAnimation = Global::GetModelFactory()->LoadAnimationFBX("Animations/A_Wizard_Idle.fbx");

		myPlayer.myAnimationPlayer.Init(myPlayer.myAnimation, myPlayer.myModel);
		myPlayer.myAnimationPlayer.SetIsLooping(true);
		myPlayer.myAnimationPlayer.Play();
	}

	void Scenes::DefaultScene::Update()
	{
		Scene::Update();
		myPlayer.myAnimationPlayer.Update();
	}

	void Scenes::DefaultScene::Render()
	{
		Scene::Render();
		Global::GetRenderer()->RenderModel(myFloor);
		Global::GetRenderer()->RenderModel(myPlayer.myModel);
	}
}