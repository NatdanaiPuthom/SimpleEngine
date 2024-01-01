#include "Game/Precomplier/stdafx.h"
#include "Game/world.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"
#include "Game/GameWorld.hpp"
#include "Game/Managers/LevelManager/LevelManager.hpp"

namespace
{
	Simple::GameWorld* localGameWorld = nullptr;
	Simple::LevelManager* localLevelManager = nullptr;

	int localActiveSceneIndex = 0;
}

void SimpleWorldGameWorldImpl::SetGameWorld(Simple::GameWorld* aGameWorld)
{
	if (localGameWorld != nullptr)
		assert(false && "localGameWorld is already set. Is this a mistake?");

	localGameWorld = aGameWorld;
}

void SimpleWorldLevelManagerImpl::SetLevelManager(Simple::LevelManager* aLevelManager)
{
	if (localLevelManager != nullptr)
		assert(false && "localLevelManager is already set. Is this a mistake?");

	localLevelManager = aLevelManager;
}

void SimpleWorld::SetActiveScene(const int aSceneIndex)
{
	localActiveSceneIndex = aSceneIndex;
	localLevelManager->SetActiveScene(localActiveSceneIndex);
}

int SimpleWorld::GetActiveSceneIndex()
{
	return localActiveSceneIndex;
}

std::shared_ptr<Scene> SimpleWorld::GetActiveScene()
{
	return localLevelManager->GetActiveScene();
}
