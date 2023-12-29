#include "Game/Precomplier/stdafx.h"
#include "Game/Managers/LevelManager/Scenes/DefaultScene.hpp"

DefaultScene::DefaultScene()
{
}

void DefaultScene::Init()
{
	ModelFactory* modelFactory = SimpleGlobal::GetModelFactory();

	{
		std::shared_ptr<ModelInstance> pyramid = std::move(modelFactory->CreatePyramidModel());
		pyramid->SetPosition({ 7.0f, 2.0f, 3.0f });
		myModelInstances.push_back(std::move(pyramid));
	}

	{
		std::shared_ptr<ModelInstance> cube = std::move(modelFactory->CreateCubeModel());
		cube->SetScale({ 5,5,5 });
		cube->SetPosition({ -7.0f, 2.0f, 10.0f });
		myModelInstances.push_back(std::move(cube));
	}

	{
		std::shared_ptr<ModelInstance> directionalLight = std::move(modelFactory->CreateDirectionalLightModel());
		directionalLight->SetPosition({ 8,6,10 });
		myModelInstances.push_back(std::move(directionalLight));
	}

	{
		std::shared_ptr<ModelInstance> sphere = std::move(modelFactory->CreateSphereModel());
		sphere->SetPosition({ 2,3,7 });
		myModelInstances.push_back(std::move(sphere));
	}

	{
		std::shared_ptr<ModelInstance> plane = std::move(modelFactory->CreatePlaneModel());
		plane->SetPosition({ -1.0f, 0, -1.0f });
		myModelInstances.push_back(std::move(plane));
	}

	{
		std::shared_ptr<ModelInstance> terrain = std::move(modelFactory->CreateTerrainModel());
		terrain->SetPosition({ -3.0f, 0.0f, 0.0f });
		myModelInstances.push_back(std::move(terrain));
	}

	{
		std::shared_ptr<ModelInstance> skyBox = std::move(modelFactory->CreateSkyBoxModel());
		skyBox->SetPosition({ 0.0f, 0.0f, 20.0f });
		myModelInstances.push_back(std::move(skyBox));
	}
}

void DefaultScene::Update()
{
	{
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

		//Sphere
		SimpleUtilities::Vector3f sphereRotation = myModelInstances[3]->GetRotation();
		sphereRotation.y += -40 * SimpleGlobal::GetDeltaTime();
		myModelInstances[3]->SetRotation(sphereRotation);
	}
}
