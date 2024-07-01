#include "Game/Precomplied/GamePch.hpp"
#include "Game/World.hpp"
#include "Game/WorldImpl/SimpleWorldImpl.hpp"
#include <cassert>

namespace World
{
	Simple::EventManager* localEventManager = nullptr;
	ECS::EntityComponentSystem* localECS = nullptr;
}

namespace World
{
	Simple::EventManager* GetEventmanager()
	{
		return localEventManager;
	}

	ECS::EntityComponentSystem* GetECS()
	{
		return localECS;
	}
}

namespace Impl
{
	void SimpleWorldEventManager::SetEventManager(Simple::EventManager* aLevelManager)
	{
		if (World::localEventManager != nullptr)
			assert(false && "localEventManager is already set. Is this a mistake?");

		World::localEventManager = aLevelManager;
	}

	void SimpleWorldECS::SetECS(ECS::EntityComponentSystem* aECS)
	{
		World::localECS = aECS;
	}
}