#include "Game/stdafx.h"
#include "Engine/Graphics/Shapes/ShapeCreator3000.hpp"
#include "Engine/Graphics/Renderer/Renderer.hpp"
#include "Game/GameWorld.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"
#include "Game/Managers/ImGuiManager/Tools/MeshTool.hpp"

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
	myImGuiManager->SetRenderer(myRenderer.get());
	myImGuiManager->AddTool(std::move(std::make_unique<MeshTool>(myRenderer.get())));

	{
		std::unique_ptr<ModelInstance> pyramid = std::make_unique<ModelInstance>();
		pyramid->Init(myRenderer->GetMesh("Pyramid"), "Assets/Textures/Cat.dds");
		pyramid->SetScale({ 1,1,1 });
		pyramid->SetPosition(SimpleUtilities::Vector3f(-8, 2, 3));
		pyramid->SetName("Pyramid");
		myRenderer->AddModelInstance(std::move(pyramid));
	}

	{
		std::unique_ptr<ModelInstance> cube = std::make_unique<ModelInstance>();
		cube->Init(myRenderer->GetMesh("Cube"));
		cube->SetScale({ 5,5,5 });
		cube->SetPosition(SimpleUtilities::Vector3f(-7, 2, 10));
		cube->SetName("Cube");
		myRenderer->AddModelInstance(std::move(cube));
	}

	{
		std::unique_ptr<ModelInstance> plane = std::make_unique<ModelInstance>();
		plane->Init(myRenderer->GetMesh("Plane"));
		plane->SetShader("Shaders/WaterReflectionPS.cso", "Shaders/DefaultVS.cso");
		plane->SetName("Plane");
		myRenderer->AddModelInstance(std::move(plane));
	}

	/*{
		SimpleUtilities::Vector3f position(-10, 0, 30);
		unsigned int count = 1;
		for (size_t x = 0; x < 10; ++x)
		{
			for (size_t y = 0; y < 10; ++y)
			{
				std::unique_ptr<ModelInstance> cube = std::make_unique<ModelInstance>();
				cube->Init(myRenderer->GetMesh("Cube"));
				cube->SetPosition(position);
				cube->SetName("Cube" + std::to_string(count));

				myRenderer->AddModelInstance(std::move(cube));

				++count;
				position.x += 5;
			}
			position.x = -10;
			position.z += 5;
		}
	}*/
}

void GameWorld::Update()
{
	while (SimpleGlobal::GetGameIsRunning())
	{
	}
}

void GameWorld::Render()
{
	myRenderer->Update(); //TO-DO: Move to Update thread later
	myRenderer->Render();
}

void GameWorld::RenderImGui()
{
	myImGuiManager->Render();
}
