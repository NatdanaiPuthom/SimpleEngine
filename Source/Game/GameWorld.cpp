#include "stdafx.h"
#include "Engine/Graphics/Shapes/ShapeCreator3000.hpp"
#include "Engine/Graphics/Renderer/Renderer.hpp"
#include "Game/GameWorld.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"

GameWorld::GameWorld()
	: myRenderer(std::make_unique<Renderer>())
	, myImGuiManager(std::make_unique<ImGuiManager>())
{
}

GameWorld::~GameWorld()
{
}

void GameWorld::Init()
{
	{ //Test stuff
		std::unique_ptr<Mesh> pyramid = std::make_unique<Mesh>();
		std::unique_ptr<Mesh> cube = std::make_unique<Mesh>();

		MeshData pyramidData = ShapeCreator3000::CreatePyramid();
		MeshData cubeData = ShapeCreator3000::CreateCube();

		if (!pyramid->Init(pyramidData))
			assert(false && "Failed to create Pyramid");

		if (!cube->Init(cubeData))
			assert(false && "Failed to create Cube");

		if (!pyramid->AddTexture(0, "Assets/Textures/Cat.dds"))
			assert(false && "Failed To Add Texture");

		pyramid->SetScale({5,5,5});
		pyramid->SetPosition(SimpleUtilities::Vector3f(-10, 0, 5));

		cube->SetScale({5,5,5});
		cube->SetPosition(SimpleUtilities::Vector3f(-5, 0, 5));

		myRenderer->AddMesh(std::move(pyramid));
		myRenderer->AddMesh(std::move(cube));
	}
}

void GameWorld::Update()
{
	while (SimplyGlobal::GetGameIsRunning())
	{
	}
}

void GameWorld::Render()
{
	myRenderer->Render();
	myImGuiManager->Render();
}
