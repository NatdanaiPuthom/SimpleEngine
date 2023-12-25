#include "Game/stdafx.h"
#include "Game/GameWorld.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"

GameWorld::GameWorld()
	: myImGuiManager(std::make_unique<ImGuiManager>())
{
}

GameWorld::~GameWorld()
{
}

void GameWorld::Init()
{
	ModelFactory* modelFactory = SimpleGlobal::GetModelFactory();

	{
		std::unique_ptr<ModelInstance> pyramid = std::move(modelFactory->CreatePyramidModel());
		pyramid->SetPosition({ -8.0f, 2.0f, 3.0f });
		myModelInstances.push_back(std::move(pyramid));
	}

	{
		std::unique_ptr<ModelInstance> cube = std::move(modelFactory->CreateCubeModel());
		cube->SetScale({ 5,5,5 });
		cube->SetPosition({ -7.0f, 2.0f, 10.0f });
		myModelInstances.push_back(std::move(cube));
	}

	{
		std::unique_ptr<ModelInstance> directionalLight = std::move(modelFactory->CreateDirectionalLightModel());
		directionalLight->SetPosition({ 8,6,10 });
		myModelInstances.push_back(std::move(directionalLight));
	}

	{
		std::unique_ptr<ModelInstance> plane = std::move(modelFactory->CreatePlaneModel());
		plane->SetScale({ 10,10,10 });
		plane->SetPosition({ 9.0f, 0.0f, 5.0f });
		myModelInstances.push_back(std::move(plane));
	}

	{
		std::unique_ptr<ModelInstance> terrain = std::move(modelFactory->CreateTerrainModel());
		terrain->SetPosition({ -3.0f, 0.0f, 0.0f });
		myModelInstances.push_back(std::move(terrain));
	}

	{
		std::unique_ptr<ModelInstance> skyBox = std::move(modelFactory->CreateSkyBoxModel());
		skyBox->SetPosition({ 0.0f, 0.0f, 20.0f });
		myModelInstances.push_back(std::move(skyBox));
	}


	{ //TO-DO: Fix better way to send data to ImGui
		std::vector<ModelInstance*> modelBuffer;
		for (auto& model : myModelInstances)
		{
			modelBuffer.push_back(model.get());
		}

		SimpleGlobal::GetRenderer()->SetModelBuffer(modelBuffer);
	}
}

void GameWorld::Update()
{
	while (SimpleGlobal::GetGameIsRunning())
	{
	}
}

void GameWorld::Render()
{
	{ //Test //Also move to Update() later
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
		myModelInstances[2]->SetRotation(directionalLight);
	}

	Renderer* renderer = SimpleGlobal::GetRenderer();
	for (const auto& model : myModelInstances)
	{
		renderer->Render(model.get());
	}
}

void GameWorld::RenderImGui()
{
	myImGuiManager->Render();
}
