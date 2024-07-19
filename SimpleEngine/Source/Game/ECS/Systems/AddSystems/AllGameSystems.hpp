#pragma once
#include "Engine/ECS/Reflection/ECSGameSystems.hpp"
#include "Game/Test/RealEnemySystem.hpp"
#include "Game/ECS/Systems/RewindSystem/RewindSystem.hpp"

namespace ECS
{
	class IECSGameSystem
	{
		friend class Simpleton::SceneManager;

		static void AddSystems() //NOTE(v11.3.1): Order of Inits, Updates and Render are based on order of AddSystem here
		{
			ECS::ECSGameSystem::AddSystem<ECS::RealEnemySystem>();
			ECS::ECSGameSystem::AddSystem<ECS::RewindSystem>();

		}
	};
}