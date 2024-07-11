#pragma once
#include "Engine/ECS/Reflection/ECSGameSystems.hpp"
#include "Game/Systems/Example/ExampleSystem.hpp"

namespace ECS
{
	class IECSGameSystem
	{
		friend class Simpleton::SceneManager;

		/*
		* NOTE(v11.2.5): Please don't touch anything else in this class other than AddSystems(). (I am playing with fire right now, we don't want oppsie accident, do we?)
		*/

		static void AddSystems()
		{
			//ECS::ECSGameSystem::AddSystem<ECS::ExampleSystem>();
		}
	};
}