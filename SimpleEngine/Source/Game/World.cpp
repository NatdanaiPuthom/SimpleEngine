#include "Game/Precomplied/GamePch.hpp"
#include "Game/World.hpp"
#include "Game/WorldImpl/SimpleWorldImpl.hpp"
#include <cassert>

namespace World
{
	Simple::EventManager* localEventManager = nullptr;
}

namespace World
{
	Simple::EventManager* GetEventmanager()
	{
		return localEventManager;
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
}