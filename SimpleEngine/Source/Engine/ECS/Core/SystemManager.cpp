#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Core/SystemManager.hpp"
#include "Engine/ECS/Core/System.hpp"

namespace ECS
{
	SystemManager::SystemManager(EntityComponentSystem& aEntityComponentSystem)
		: myFixedUpdateTime(1.0f / 60.0f)
		, myTimer(0.0f)
		, mySkyBoxAndDirectionalLightSystem(aEntityComponentSystem)
	{
	}

	SystemManager::~SystemManager()
	{
	}

	void SystemManager::Init()
	{
		for (auto& system : mySystems)
		{
			system->Init();
		}

		mySkyBoxAndDirectionalLightSystem.Init();
	}

	void SystemManager::Update()
	{
		myTimer += Global::GetDeltaTime();
		const bool shouldUpdate = myTimer > myFixedUpdateTime;

		if (shouldUpdate == true)
		{
			myTimer = 0.0f;
		}

		for (auto& system : mySystems)
		{
			system->EarlyUpdate();
		}

		for (auto& system : mySystems)
		{
			if (shouldUpdate)
			{
				system->FixedUpdate();
			}
		}

		for (auto& system : mySystems)
		{
			system->Update();
			mySkyBoxAndDirectionalLightSystem.Update();
		}

		for (auto& system : mySystems)
		{
			system->LateUpdate();
		}
	}

	void SystemManager::Render()
	{
		for (auto& system : mySystems)
		{
			system->Render();
		}
	}

	void SystemManager::RenderPointLights()
	{
		mySkyBoxAndDirectionalLightSystem.Render();
	}

	void SystemManager::RenderSkyBoxAndDirectionalLight()
	{
		mySkyBoxAndDirectionalLightSystem.RenderSkyBoxAndDirectionalLight();
	}

	void SystemManager::AddSystem(std::unique_ptr<System> aSystem)
	{
		mySystems.push_back(std::move(aSystem));
	}
}