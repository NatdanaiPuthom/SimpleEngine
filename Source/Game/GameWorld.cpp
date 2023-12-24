#include "Game/stdafx.h"
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
	ModelFactory* modelFactory = SimpleGlobal::GetModelFactory();

	myImGuiManager->SetRenderer(renderer);
	myImGuiManager->AddTool(std::move(std::make_unique<MeshTool>(renderer)));

	{
		std::unique_ptr<ModelInstance> pyramid = std::make_unique<ModelInstance>();
		pyramid->Init(modelFactory->GetMesh("Pyramid"), "Assets/Textures/Cat.dds");
		pyramid->SetScale({ 1,1,1 });
		pyramid->SetPosition(SimpleUtilities::Vector3f(-8, 2, 3));
		pyramid->SetName("Pyramid");
		myModelInstances.push_back(std::move(pyramid));
	}

	{
		std::unique_ptr<ModelInstance> cube = std::make_unique<ModelInstance>();
		cube->Init(modelFactory->GetMesh("Cube"));
		cube->SetScale({ 5,5,5 });
		cube->SetPosition(SimpleUtilities::Vector3f(-7, 2, 10));
		cube->SetName("Cube");
		myModelInstances.push_back(std::move(cube));
	}

	{
		std::unique_ptr<ModelInstance> plane = std::make_unique<ModelInstance>();
		plane->Init(modelFactory->GetMesh("Plane"));
		plane->SetShader("Shaders/WaterReflectionPS.cso", "Shaders/DefaultVS.cso");
		plane->SetName("Plane");
		myModelInstances.push_back(std::move(plane));
	}

	myModelInstances.push_back(std::move(modelFactory->CreateTerrainModel()));
	myModelInstances.push_back(std::move(modelFactory->CreateSkyBoxModel()));
	myDirectionalLight = std::move(modelFactory->CreateDirectionalLightModel());
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

	renderer->Render(*myDirectionalLight);
}

void GameWorld::RenderImGui()
{
	myImGuiManager->Render();
}
