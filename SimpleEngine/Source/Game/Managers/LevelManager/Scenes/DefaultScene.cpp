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
	}

	void Scenes::DefaultScene::Init()
	{
		myDirectionalLight->myModel->SetPosition({ 0.0f, 10.0f, 1.0f });
		Global::GetGraphicsEngine()->SetDirectionalLightDirection({ 0.0f, -1.0f, 0.0f });
	}

	void Scenes::DefaultScene::Update()
	{
		Scene::Update();
	}

	void Scenes::DefaultScene::Render()
	{
		Scene::Render();
	}
}