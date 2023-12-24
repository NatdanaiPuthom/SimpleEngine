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

	myModelInstances.push_back(std::move(modelFactory->CreatePyramidModel()));
	myModelInstances.push_back(std::move(modelFactory->CreateCubeModel()));
	myModelInstances.push_back(std::move(modelFactory->CreatePlaneModel()));
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
	{ //Test
		//Pyramid
		SimpleUtilities::Vector3f pyramidRotation = myModelInstances[0]->GetRotation();
		pyramidRotation.y += -10 * SimpleGlobal::GetDeltaTime();
		pyramidRotation.z += 10 * SimpleGlobal::GetDeltaTime();
		myModelInstances[0]->SetRotation(pyramidRotation);

		//Cube
		SimpleUtilities::Vector3f cubeRotation = myModelInstances[1]->GetRotation();
		cubeRotation.x += 10.0f * SimpleGlobal::GetDeltaTime();
		myModelInstances[1]->SetRotation(cubeRotation);

		//Directional Light test
		SimpleUtilities::Vector3f directionalLight = SimpleGlobal::GetGraphicsEngine()->GetDirectionalLightDirection() * 180.0f;
		myDirectionalLight->SetRotation(directionalLight);
	}

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
