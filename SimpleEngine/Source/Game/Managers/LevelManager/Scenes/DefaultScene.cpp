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
		auto camera = Global::GetGraphicsEngine()->GetEditorCamera();
		camera->SetRotation(Math::Vector3f(30, 0, 0));
		camera->SetPosition(Math::Vector3f(-6, 6, -9));

		myDirectionalLight->myModel->SetPosition({ -8.0f, 1.0f, 1.0f });
		Global::GetGraphicsEngine()->SetDirectionalLightDirection({ 0.0f, 0.0f, -1.0f });
	}

	void Scenes::DefaultScene::Init()
	{
		myFloor = Global::GetModelFactory()->LoadStaticModelFBX("StaticModels/Simple_Floor_10x10.fbx");
		myFloor.SetPosition({ 0.0f,-4.0f, 0.0f });

		myPlayer.Init();

		myIKTest.Init();
	}

	void Scenes::DefaultScene::Update()
	{
		Scene::Update();
		myPlayer.Update();

		myIKTest.Update();
	}

	void Scenes::DefaultScene::Render()
	{
		Scene::Render();

		//myIKTest.Render();

		//myPlayer.Render();
		//auto renderer = Global::GetRenderer();
		//renderer->RenderModel(myFloor);
		//renderer->RenderBoundingBox(myFloor);
	}
}