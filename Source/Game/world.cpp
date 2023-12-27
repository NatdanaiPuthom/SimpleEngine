#include "Game/Precomplier/stdafx.h"
#include "Game/world.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"
#include "Game/GameWorld.hpp"
#include "Game/Managers/LevelManager/LevelManager.hpp"

namespace
{
	GameWorld* localGameWorld = nullptr;
	LevelManager* localLevelManager = nullptr;

	int localActiveSceneIndex = 0;
}

void SimpleWorldGameWorldImpl::SetGameWorld(GameWorld* aGameWorld)
{
	localGameWorld = aGameWorld;
}

void SimpleWorldLevelManagerImpl::SetLevelManager(LevelManager* aLevelManager)
{
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
