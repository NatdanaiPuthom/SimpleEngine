#pragma once
#include "Engine/ECS/ECS.hpp"
#include "Game/Managers/EventManager/EventManager.hpp"

namespace World
{
	Simple::EventManager* GetEventmanager();
	ECS::EntityComponentSystem* GetECS();
}