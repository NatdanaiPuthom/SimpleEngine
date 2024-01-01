#include "Game/Precomplier/stdafx.h"
#include "Game/Managers/LevelManager/Scenes/DefaultScene.hpp"

DefaultScene::DefaultScene()
{
	myDirectionalLight->myModel->SetPosition({ 0.0f, 7.0f, -5.0f });
	SimpleGlobal::GetGraphicsEngine()->SetDirectionalLightDirection({ -1.0f, -1.0f, 1.0f });
}

void DefaultScene::Init()
{
	ModelFactory* modelFactory = SimpleGlobal::GetModelFactory();

	{
		std::shared_ptr<Model> pyramid = std::move(modelFactory->CreatePyramidModel());
		pyramid->SetPosition({ 7.0f, 5.0f, 3.0f });
		myModels.push_back(std::move(pyramid));
	}

	{
		std::shared_ptr<Model> cube = std::move(modelFactory->CreateCubeModel());
		cube->ClearTextures();
		cube->AddTexture("Assets/Textures/Hamster.dds");
		cube->SetName("Hamster");
		cube->SetPosition({-5,3,0});
		cube->SetShader("Shaders/DefaultPBRPS.cso", "Shaders/DefaultVS.cso");
		myModels.push_back(std::move(cube));
	}

	{
		std::shared_ptr<Model> cube = std::move(modelFactory->CreateCubeModel());
		cube->SetScale({ 2,2,2 });
		cube->SetPosition({ -7.0f, 2.0f, 10.0f });
		myModels.push_back(std::move(cube));
	}

	{
		std::shared_ptr<Model> sphere = std::move(modelFactory->CreateSphereModel());
		sphere->SetPosition({ 2,5,7 });
		myModels.push_back(std::move(sphere));
	}

	{
		std::shared_ptr<Model> plane = std::move(modelFactory->CreatePlaneModel());
		plane->SetPosition({ -1.0f, 0, -2.0f });
		plane->SetName("Cat");
		plane->ClearTextures();
		plane->AddTexture("Assets/Textures/Cat-scared.dds");
		plane->SetShader("Shaders/DefaultPBRPS.cso", "Shaders/DefaultVS.cso");
		myModels.push_back(std::move(plane));
	}

	{
		std::shared_ptr<Model> terrain = std::move(modelFactory->CreateTerrainModel());
		terrain->SetPosition({ -3.0f, 0.0f, 0.0f });
		myModels.push_back(std::move(terrain));
	}

	{
		std::shared_ptr<Model> cube = std::move(modelFactory->CreateCubeModel());
		cube->SetShader("Shaders/DefaultColorfulPS.cso", "Shaders/DefaultVS.cso");
		cube->SetName("Colorful");
		cube->SetPosition({ -5.0f, 0.5f, 0.0f });
		myModels.push_back(std::move(cube));
	}

	{
		std::shared_ptr<Model> pyramid = std::move(modelFactory->CreatePyramidModel());
		pyramid->SetPosition({ -10.0f, 3.0f, -0.0f });
		pyramid->ClearTextures();
		pyramid->AddTexture("Assets/Textures/DefaultTexture.dds");
		pyramid->SetShader("Shaders/DefaultPBRPS.cso", "Shaders/DefaultVS.cso");
		myModels.push_back(std::move(pyramid));
	}

	/*{
		std::shared_ptr<ModelInstance> skyBox = std::move(modelFactory->CreateSkyBoxModel());
		skyBox->SetPosition({ 0.0f, 0.0f, 20.0f });
		myModelInstances.push_back(std::move(skyBox));
	}*/
}

void DefaultScene::Update()
{
	Scene::Update();
}