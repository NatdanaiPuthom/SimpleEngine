#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/Scenes/SpotlightSceneTest.hpp"

namespace Scenes
{
	SpotlightScene::SpotlightScene()
	{
	}

	void SpotlightScene::Init()
	{
		myDirectionalLight->myModel->SetPosition({ 0.0f, 10.0f, 1.0f });
		Global::GetGraphicsEngine()->SetDirectionalLightDirection({ 0.0f, -1.0f, 0.0f });

		Simple::ModelFactory* modelFactory = Global::GetModelFactory();

		{
			std::shared_ptr<Simple::Model> plane = std::move(modelFactory->CreatePlaneModel());
			plane->SetPosition({ 0.0f, -3.0f, 0.0f });
			plane->SetScale({ 100,1,100 });
			plane->SetShader("DefaultPBRPS.cso", "DefaultVS.cso");
			//myModels.push_back(std::move(plane));
		}

		{
			std::shared_ptr<Simple::Model> sphere = std::move(modelFactory->CreateSphereModel());
			sphere->SetPosition({ 5,3,0 });
			sphere->SetName("Spotlight1");
			myModels.push_back(sphere);
		}

		/*int index = 1;
		SimpleUtilities::Vector3f position(0, 5, 5);
		for (size_t x = 0; x < 10; ++x)
		{
			for (size_t y = 0; y < 10; ++y)
			{
				std::shared_ptr<Simple::Model> sphere = std::move(modelFactory->CreateSphereModel());
				sphere->SetPosition(position);
				sphere->SetName("Sphere" + std::to_string(index));
				myModels.push_back(sphere);

				position.x += 5;
				++index;
			}
			position.x = 0;
			position.z += 5;
		}*/
	}

	void SpotlightScene::Update()
	{
		Scene::Update();

		for (size_t i = 2; i < myModels.size(); ++i)
		{
			Math::Vector3f newRotation = myModels[i]->GetRotation();
			newRotation.y += 40 * Global::GetDeltaTime();
			myModels[i]->SetRotation(newRotation);
		}
	}
}