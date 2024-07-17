#pragma once
#include "Engine/ECS/Reflection/ECSGameSystems.hpp"
#include "Game/Systems/Example/ExampleSystem.hpp"
#include "Game/Test/RealEnemySystem.hpp"
#include "Game/Systems/AddSystemHere/RewindSystem.hpp"

namespace ECS
{
	class IECSGameSystem
	{
		friend class Simpleton::SceneManager;

		static void AddSystems() //NOTE(v11.3.1): Order of Inits, Updates and Render are based on order of AddSystem here
		{
			//ECS::ECSGameSystem::AddSystem<ECS::ExampleSystem>();
			ECS::ECSGameSystem::AddSystem<ECS::RealEnemySystem>();
			ECS::ECSGameSystem::AddSystem<ECS::RewindSystem>();

		}
	};
}