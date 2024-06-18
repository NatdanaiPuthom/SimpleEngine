#include "Game/Precomplied/GamePch.hpp"
//#include "Game/Managers/LevelManager/Scenes/DefaultScene.hpp"
//
//namespace Scenes
//{
//	Scenes::DefaultScene::DefaultScene()
//	{
//	}
//
//	Scenes::DefaultScene::~DefaultScene()
//	{
//	}
//
//	void DefaultScene::OnEnter()
//	{
//		auto camera = Global::GetGraphicsEngine()->GetEditorCamera();
//		camera->SetRotation(Math::Vector3f(30, 0, 0));
//		camera->SetPosition(Math::Vector3f(1, 9, -12));
//
//		Global::GetGraphicsEngine()->SetDirectionalLightDirection({ 0.0f, 0.0f, -1.0f });
//	}
//
//	void Scenes::DefaultScene::Init()
//	{
//		myPlayer.Init();
//	}
//
//	void Scenes::DefaultScene::Update()
//	{
//		Scene::Update();
//		myPlayer.Update();
//	}
//
//	void Scenes::DefaultScene::Render()
//	{
//		Scene::Render();
//		//myPlayer.Render();
//	}
//}