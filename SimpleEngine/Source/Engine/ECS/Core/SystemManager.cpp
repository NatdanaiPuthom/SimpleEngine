#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Core/SystemManager.hpp"
#include "Engine/ECS/Core/System.hpp"
#include "Engine/ECS/Systems/AnimationSystem.hpp"

namespace ECS
{
	SystemManager::SystemManager(EntityComponentSystem* aEntityComponentSystem)
	{
		mySkyBoxAndDirectionalLightSystem = std::make_shared<RenderLightSystem>(aEntityComponentSystem);

		AddSystem<RenderSystem>(aEntityComponentSystem);
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
	}

	void SystemManager::FixedUpdate()
	{
		for (const auto& [key, system] : mySystems)
		{
			system->FixedUpdate();
		}

		mySkyBoxAndDirectionalLightSystem->FixedUpdate();
	}

	void SystemManager::LateUpdate()
	{
		for (const auto& [key, system] : mySystems)
		{
			system->LateUpdate();
		}

		mySkyBoxAndDirectionalLightSystem->LateUpdate();
	}

	void SystemManager::LateRender()
	{
		for (const auto& [key, system] : mySystems)
		{
			system->LateRender();
		}
	}

	void SystemManager::UpdateRenderSystem()
	{
		mySkyBoxAndDirectionalLightSystem->Update();
	}

	void SystemManager::Render()
	{
		for (const auto& [key, system] : mySystems)
		{
			system->Render();
		}
	}
}