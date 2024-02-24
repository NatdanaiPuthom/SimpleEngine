#include "Game/Precomplied/GamePch.hpp"
#include "Game/Managers/LevelManager/Scenes/SpriteTest.hpp"

Scenes::SpriteTestScene::SpriteTestScene()
{
}

Scenes::SpriteTestScene::~SpriteTestScene()
{
}

void Scenes::SpriteTestScene::Init()
{
	mySprite.position = { 0.5f, 0.5f };
	mySprite.size = { 100.0f, 100.0f };
	mySprite.texture = Global::GetGraphicsEngine()->GetTexture("Cat.dds");
}

void Scenes::SpriteTestScene::Update()
{
	Scene::Update();
}

void Scenes::SpriteTestScene::Render()
{
	Scene::Render();

	Global::GetRenderer()->RenderSprite2D(mySprite);
}