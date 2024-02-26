#include "Game/Precomplied/GamePch.hpp"
#include "Graphics/Model/Special/PlaneReflection.h"
#include "Game/Managers/LevelManager/Scenes/DefaultScene.hpp"

namespace Scenes
{
	DefaultScene::DefaultScene()
	{
		myDirectionalLight->myModel->SetPosition({ 0.0f, 7.0f, -5.0f });
		Global::GetGraphicsEngine()->SetDirectionalLightDirection({ -0.123f, 0.053f, 1.0f });
	}

	DefaultScene::~DefaultScene()
	{
		World::SetWaterPlane(nullptr);
	}

	void DefaultScene::Init()
	{
		myPlaneReflection = std::make_unique<Simple::PlaneReflection>();
		World::SetWaterPlane(myPlaneReflection.get());

		Simple::ModelFactory* modelFactory = Global::GetModelFactory();

		{
			std::shared_ptr<Simple::Model> pyramid = std::move(modelFactory->CreatePyramidModel());
			pyramid->SetPosition({ 7.0f, 5.0f, 3.0f });
			myModels.push_back(std::move(pyramid));
		}

		{
			std::shared_ptr<Simple::Model> cube = std::move(modelFactory->CreateCubeModel());
			cube->ClearTextures();
			cube->AddTexture("Hamster.dds");
			cube->SetName("Hamster");
			cube->SetPosition({ -5,3,0 });
			cube->SetShader("DefaultPBRPS.cso", "DefaultVS.cso");
			myModels.push_back(std::move(cube));
		}

		{
			std::shared_ptr<Simple::Model> cube = std::move(modelFactory->CreateCubeModel());
			cube->SetScale({ 2,2,2 });
			cube->SetPosition({ -7.0f, 2.0f, 10.0f });
			myModels.push_back(std::move(cube));
		}

		{
			std::shared_ptr<Simple::Model> sphere = std::move(modelFactory->CreateSphereModel());
			sphere->SetPosition({ 2,5,7 });
			myModels.push_back(std::move(sphere));
		}

		{
			std::shared_ptr<Simple::Model> plane = std::move(modelFactory->CreatePlaneModel());
			plane->SetPosition({ -1.0f, 0, -2.0f });
			plane->SetName("Cat");
			plane->ClearTextures();
			plane->AddTexture("Cat-scared.dds");
			plane->SetShader("DefaultPBRPS.cso", "DefaultVS.cso");
			myModels.push_back(std::move(plane));
		}

		{
			std::shared_ptr<Simple::Model> terrain = std::move(modelFactory->CreateTerrainModel());
			terrain->SetPosition({ -3.0f, -0.6f, 0.0f });
			myModels.push_back(std::move(terrain));
		}

		{
			std::shared_ptr<Simple::Model> cube = std::move(modelFactory->CreateCubeModel());
			cube->SetShader("DefaultColorfulPS.cso", "DefaultVS.cso");
			cube->SetName("Colorful");
			cube->SetPosition({ -5.0f, 0.5f, 0.0f });
			myModels.push_back(std::move(cube));
		}

		{
			std::shared_ptr<Simple::Model> pyramid = std::move(modelFactory->CreatePyramidModel());
			pyramid->SetPosition({ -10.0f, 3.0f, -0.0f });
			pyramid->ClearTextures();
			pyramid->AddTexture("DefaultTexture.dds");
			pyramid->SetShader("DefaultPBRPS.cso", "DefaultVS.cso");
			myModels.push_back(std::move(pyramid));
		}

		{
			std::shared_ptr<Simple::Model> skyBox = std::move(modelFactory->CreateSkyBoxModel());
			skyBox->SetPosition({ 0.0f, 0.0f, 20.0f });
			myModels.push_back(std::move(skyBox));
		}
	}

	void DefaultScene::Update()
	{
		Simple::Scene::Update();
		myPlaneReflection->Update();

		auto camPosition = Global::GetGraphicsEngine()->GetCurrentCamera()->GetPosition();
		camPosition.x += 5.0f;
		myModels[2]->LookAt(camPosition);

		auto rotation = myModels[1]->GetRotation();
		rotation.y += 10 * Global::GetDeltaTime();
		myModels[1]->SetRotation(rotation);

		rotation = myModels[4]->GetRotation();
		rotation.y -= 40 * Global::GetDeltaTime();
		myModels[4]->SetRotation(rotation);
	}

	void DefaultScene::Render()
	{
		Scene::Render();

		Global::GetRenderer()->RenderPlaneReflection(World::GetWaterPlane()->myModel);
	}
}