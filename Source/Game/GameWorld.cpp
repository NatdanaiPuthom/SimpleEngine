#include "Game/stdafx.h"
#include "Engine/Graphics/Renderer/Renderer.hpp"
#include "Game/GameWorld.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"
#include "Game/Managers/ImGuiManager/Tools/MeshTool.hpp"

GameWorld::GameWorld()
	: myImGuiManager(std::make_unique<ImGuiManager>())
{
}

GameWorld::~GameWorld()
{
}

void GameWorld::Init()
{
	Renderer* renderer = SimpleGlobal::GetRenderer();

	myImGuiManager->SetRenderer(renderer);
	myImGuiManager->AddTool(std::move(std::make_unique<MeshTool>(renderer)));

	/*{
		std::unique_ptr<ModelInstance> pyramid = std::make_unique<ModelInstance>();
		pyramid->Init(renderer->GetMesh("Pyramid"), "Assets/Textures/Cat.dds");
		pyramid->SetScale({ 1,1,1 });
		pyramid->SetPosition(SimpleUtilities::Vector3f(-8, 2, 3));
		pyramid->SetName("Pyramid");
		renderer->AddModelInstance(std::move(pyramid));
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
	}*/

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
	Renderer* renderer = SimpleGlobal::GetRenderer();

	for (const auto& model : myModelInstances)
	{
		renderer->Render(*model);
	}
}

void GameWorld::RenderImGui()
{
	myImGuiManager->Render();
}
