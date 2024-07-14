#pragma once
#include "Engine/ECS/Reflection/ECSGameSystems.hpp"
#include "Game/Systems/Example/ExampleSystem.hpp"

namespace ECS
{
	class IECSGameSystem
	{
		friend class Simpleton::SceneManager;

		static void AddSystems()
		{
			//ECS::ECSGameSystem::AddSystem<ECS::ExampleSystem>();
		}
	};
}