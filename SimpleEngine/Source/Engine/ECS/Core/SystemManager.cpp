#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Core/SystemManager.hpp"
#include "Engine/ECS/Core/System.hpp"
#include "Engine/ECS/Systems/AnimationSystem.hpp"

namespace ECS
{
	SystemManager::SystemManager(EntityComponentSystem* aEntityComponentSystem)
	{
		mySkyBoxAndDirectionalLightSystem = std::make_shared<RenderLightSystem>(aEntityComponentSystem);
		myRenderSystem = std::make_shared<RenderSystem>(aEntityComponentSystem);

		AddSystem<AnimationSystem>(aEntityComponentSystem);
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
		myRenderSystem->Init();
	}

	void SystemManager::Update()
	{
		for (const auto& [key, system] : mySystems)
		{
			system->Update();
		}
	}

	void SystemManager::EarlyUpdate()
	{
		for (const auto& [key, system] : mySystems)
		{
			system->EarlyUpdate();
		}

		mySkyBoxAndDirectionalLightSystem->EarlyUpdate();
		myRenderSystem->EarlyUpdate();
	}

	void SystemManager::FixedUpdate()
	{
		for (const auto& [key, system] : mySystems)
		{
			system->FixedUpdate();
		}

		mySkyBoxAndDirectionalLightSystem->FixedUpdate();
		myRenderSystem->FixedUpdate();
	}

	void SystemManager::LateUpdate()
	{
		for (const auto& [key, system] : mySystems)
		{
			system->LateUpdate();
		}

		mySkyBoxAndDirectionalLightSystem->LateUpdate();
		myRenderSystem->LateUpdate();
	}

	void SystemManager::UpdateRenderSystem()
	{
		mySkyBoxAndDirectionalLightSystem->Update();
		myRenderSystem->Update();
	}

	void SystemManager::Render()
	{
		myRenderSystem->Render();

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