#include "Game/stdafx.h"
#include "Game/world.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"
#include "Game/GameWorld.hpp"

namespace
{
	GameWorld* localGameWorld = nullptr;

	int localActiveSceneIndex = 0;
}

void SimpleWorldGameWorldImpl::SetGameWorld(GameWorld* aGameWorld)
{
	localGameWorld = aGameWorld;
}

void SimpleWorld::SetActiveScene(const int aSceneIndex)
{
	localActiveSceneIndex = aSceneIndex;
	localGameWorld->SetActiveScene(localActiveSceneIndex);
}

std::shared_ptr<Scene> SimpleWorld::GetActiveScene()
{
	return localGameWorld->GetActiveScene();
}
