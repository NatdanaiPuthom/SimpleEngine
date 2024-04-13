#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/ECS/Disgusting/ECSNeedFix.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"

namespace Disgusting
{
	Simple::ComponentManager* localComponentManager = nullptr;

	Simple::ComponentManager* GetComponentManager()
	{
		return localComponentManager;
	}

	void DisgustingWorldComponentManager::SetComponentManager(Simple::ComponentManager* aComponentManager)
	{
		if (localComponentManager != nullptr)
			assert(false && "localComponentManager is already set. Is this a mistake?");

		localComponentManager = aComponentManager;
	}
}