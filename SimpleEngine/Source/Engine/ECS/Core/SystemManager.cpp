#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Core/SystemManager.hpp"
#include "Engine/ECS/Core/System.hpp"

namespace ECS
{
	SystemManager::SystemManager(EntityComponentSystem* aEntityComponentSystem)
		: myFixedUpdateTime(1.0f / 60.0f)
		, myTimer(0.0f)
	{
		mySkyBoxAndDirectionalLightSystem = std::make_shared<RenderLightSystem>(aEntityComponentSystem);
	}

	SystemManager::~SystemManager()
	{
	}

	void SystemManager::Init()
	{
		for (const auto& [key, system] : mySystems)
		{
			system->Init();
		}

		mySkyBoxAndDirectionalLightSystem->Init();
	}

	void SystemManager::Update()
	{
		myTimer += Global::GetDeltaTime();
		const bool shouldUpdate = myTimer > myFixedUpdateTime;

		if (shouldUpdate == true)
		{
			myTimer = 0.0f;
		}

		for (const auto& [key, system] : mySystems)
		{
			system->EarlyUpdate();
		}

		for (const auto& [key, system] : mySystems)
		{
			if (shouldUpdate)
			{
				system->FixedUpdate();
			}
		}

		for (const auto& [key, system] : mySystems)
		{
			system->Update();
			mySkyBoxAndDirectionalLightSystem->Update();
		}

		for (const auto& [key, system] : mySystems)
		{
			system->LateUpdate();
		}
	}

	void SystemManager::Render()
	{
		for (const auto& [key, system] : mySystems)
		{
			system->Render();
		}
	}

	void SystemManager::RenderPointLights()
	{
		mySkyBoxAndDirectionalLightSystem->Render();
	}

	void SystemManager::RenderSkyBoxAndDirectionalLight()
	{
		mySkyBoxAndDirectionalLightSystem->RenderSkyBoxAndDirectionalLight();
	}
}