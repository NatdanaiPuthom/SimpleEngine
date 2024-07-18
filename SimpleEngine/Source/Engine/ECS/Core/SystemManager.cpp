#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Core/SystemManager.hpp"
#include "Engine/ECS/Core/System.hpp"
#include "Engine/ECS/Systems/AnimationSystem.hpp"
#include "Engine/ECS/Systems/ScriptSystem.hpp"

namespace ECS
{
	SystemManager::SystemManager(EntityComponentSystem* aEntityComponentSystem)
	{
		AddSystem<RenderSystem>(aEntityComponentSystem);
		AddSystem<RenderLightSystem>(aEntityComponentSystem);
		AddSystem<AnimationSystem>(aEntityComponentSystem);
		AddSystem<ScriptSystem>(aEntityComponentSystem);
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
	}

	void SystemManager::FixedUpdate()
	{
		for (const auto& [key, system] : mySystems)
		{
			system->FixedUpdate();
		}
	}

	void SystemManager::LateUpdate()
	{
		for (const auto& [key, system] : mySystems)
		{
			system->LateUpdate();
		}
	}

	void SystemManager::LateRender()
	{
		for (const auto& [key, system] : mySystems)
		{
			system->LateRender();
		}
	}

	void SystemManager::Render()
	{
		for (const auto& [key, system] : mySystems)
		{
			system->Render();
		}
	}
}