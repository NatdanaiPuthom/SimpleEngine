#include "Game/Precomplier/stdafx.h"
#include "Game/world.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"
#include "Game/GameWorld.hpp"
#include "Game/Managers/LevelManager/LevelManager.hpp"
#include "Game/Managers/LevelManager/Template/Scene.hpp"

namespace
{
	Simple::GameWorld* localGameWorld = nullptr;
	Simple::LevelManager* localLevelManager = nullptr;
	Simple::PlaneReflection* localWaterPlane = nullptr;

	float localWaterHeight = 0;
	int localActiveSceneIndex = 0;
}

void Impl::SimpleWorldGameWorld::SetGameWorld(Simple::GameWorld* aGameWorld)
{
	if (localGameWorld != nullptr)
		assert(false && "localGameWorld is already set. Is this a mistake?");

	localGameWorld = aGameWorld;
}

void Impl::SimpleWorldLevelManager::SetLevelManager(Simple::LevelManager* aLevelManager)
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

void SimpleWorld::SetWaterHeight(const float aHeight)
{
	localWaterHeight = aHeight;
}

void SimpleWorld::SetWaterPlane(Simple::PlaneReflection* aWaterPlane)
{
	localWaterPlane = aWaterPlane;
}

float SimpleWorld::GetWaterHeight()
{
	return localWaterHeight;
}

int SimpleWorld::GetActiveSceneIndex()
{
	return localActiveSceneIndex;
}

std::shared_ptr<Simple::Scene> SimpleWorld::GetActiveScene()
{
	return localLevelManager->GetActiveScene();
}

Simple::PlaneReflection* SimpleWorld::GetWaterPlane()
{
	return localWaterPlane;
}