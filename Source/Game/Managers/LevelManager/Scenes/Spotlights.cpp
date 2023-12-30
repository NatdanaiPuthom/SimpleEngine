#include "Game/Precomplier/stdafx.h"
#include "Game/Managers/LevelManager/Scenes/Spotlights.hpp"

SpotlightScene::SpotlightScene()
{
}

void SpotlightScene::Init()
{
	ModelFactory* modelFactory = SimpleGlobal::GetModelFactory();

	{
		std::shared_ptr<ModelInstance> terrain = std::move(modelFactory->CreateTerrainModel());
		terrain->SetPosition({ -5.0f, 0.0f, 0.0f });
		terrain->SetScale({ 3,2,3 });
		myModelInstances.push_back(std::move(terrain));
	}

	int index = 1;
	SimpleUtilities::Vector3f position(0, 5, 5);
	for (size_t x = 0; x < 10; ++x)
	{
		for (size_t y = 0; y < 10; ++y)
		{
			std::shared_ptr<ModelInstance> sphere = std::move(modelFactory->CreateSphereModel());
			sphere->SetPosition(position);
			sphere->SetName("Sphere" + std::to_string(index));
			myModelInstances.push_back(sphere);

			position.x += 5;
			++index;
		}
		position.x = 0;
		position.z += 5;
	}
}

void SpotlightScene::Update()
{
	Scene::Update();

	for (size_t i = 2; i < myModelInstances.size(); ++i)
	{
		SimpleUtilities::Vector3f newRotation = myModelInstances[i]->GetRotation();
		newRotation.y += 40 * SimpleGlobal::GetDeltaTime();
		myModelInstances[i]->SetRotation(newRotation);
	}
}
