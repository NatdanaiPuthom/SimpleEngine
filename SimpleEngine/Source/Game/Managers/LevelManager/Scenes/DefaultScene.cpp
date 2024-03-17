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
		myPlayer.Init();
	}

	void Scenes::DefaultScene::Update()
	{
		Scene::Update();
		myPlayer.Update();
	}

	void Scenes::DefaultScene::Render()
	{
		Scene::Render();

		auto renderer = Global::GetRenderer();

		myPlayer.Render();
		renderer->RenderModel(myFloor);
		renderer->RenderBoundingBox(myFloor);
	}
}