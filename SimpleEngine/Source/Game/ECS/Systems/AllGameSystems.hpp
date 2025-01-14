#pragma once
#include "Engine/ECS/Reflection/ECSGameSystems.hpp"
#include "Game/ECS/Systems/AddSystems/BasicPlayerSystem.hpp"

namespace ECS
{
	class IECSGameSystem
	{
		friend class Simpleton::SceneManager;

		static void AddSystems() //NOTE(v11.3.1): Order of Init, Update and Render are based on order of AddSystem here
		{
			ECS::ECSGameSystem::AddSystem<ECS::BasicPlayerSystem>();
		}
	};
}