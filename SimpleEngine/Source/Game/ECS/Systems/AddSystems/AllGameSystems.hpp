#pragma once
#include "Engine/ECS/Reflection/ECSGameSystems.hpp"

namespace ECS
{
	class IECSGameSystem
	{
		friend class Simpleton::SceneManager;

		static void AddSystems() //NOTE(v11.3.1): Order of Inits, Updates and Render are based on order of AddSystem here
		{

		}
	};
}