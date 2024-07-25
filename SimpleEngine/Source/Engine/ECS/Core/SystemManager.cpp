#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/ECS/Core/SystemManager.hpp"
#include "Engine/ECS/Core/System.hpp"
#include "Engine/ECS/Systems/AnimationSystem.hpp"
#include "Engine/ECS/Systems/ScriptSystem.hpp"
#include "Engine/ECS/Systems/CameraSystem.hpp"

namespace ECS
{
	SystemManager::SystemManager()
	{
		AddSystem<RenderSystem>();
		AddSystem<RenderLightSystem>();
		AddSystem<CameraSystem>();
		AddSystem<AnimationSystem>();
		AddSystem<ScriptSystem>();
	}

	SystemManager::~SystemManager()
	{
	}

	SystemManager::SystemManager(const SystemManager& aOther)
	{
		for (const auto& [key,system] : aOther.mySystems)
		{
			mySystems.emplace(key, system->Clone());
		}
	}

	SystemManager::SystemManager(SystemManager&& aOther) noexcept = default;

	SystemManager& SystemManager::operator=(const SystemManager& aOther)
	{
		mySystems.clear();

		for (const auto& [key, system] : aOther.mySystems)
		{
			mySystems.emplace(key, system->Clone());
		}

		return *this;
	}

	SystemManager& SystemManager::operator=(SystemManager&& aOther) noexcept = default;

	void SystemManager::Init(EntityComponentSystem* aEntityComponentSystem)
	{
		for (const auto& [key, system] : mySystems)
		{
			system->Init(aEntityComponentSystem);
		}
	}

	void SystemManager::Update(EntityComponentSystem* aEntityComponentSystem)
	{
		for (const auto& [key, system] : mySystems)
		{
			system->Update(aEntityComponentSystem);
		}
	}

	void SystemManager::EarlyUpdate(EntityComponentSystem* aEntityComponentSystem)
	{
		for (const auto& [key, system] : mySystems)
		{
			system->EarlyUpdate(aEntityComponentSystem);
		}
	}

	void SystemManager::FixedUpdate(EntityComponentSystem* aEntityComponentSystem)
	{
		for (const auto& [key, system] : mySystems)
		{
			system->FixedUpdate(aEntityComponentSystem);
		}
	}

	void SystemManager::LateUpdate(EntityComponentSystem* aEntityComponentSystem)
	{
		for (const auto& [key, system] : mySystems)
		{
			system->LateUpdate(aEntityComponentSystem);
		}
	}

	void SystemManager::LateRender(EntityComponentSystem* aEntityComponentSystem)
	{
		for (const auto& [key, system] : mySystems)
		{
			system->LateRender(aEntityComponentSystem);
		}
	}

	void SystemManager::Render(EntityComponentSystem* aEntityComponentSystem)
	{
		for (const auto& [key, system] : mySystems)
		{
			system->Render(aEntityComponentSystem);
		}
	}
}