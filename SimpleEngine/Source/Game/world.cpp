#include "Game/Precomplied/GamePch.hpp"
#include "Game/world.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"
#include "Game/GameWorld.hpp"
#include "Game/Managers/LevelManager/LevelManager.hpp"
#include "Game/Managers/EventManager/EventManager.hpp"
#include "Game/Managers/LevelManager/Template/Scene.hpp"
#include <cassert>

namespace
{
	Simple::GameWorld* localGameWorld = nullptr;
	Simple::LevelManager* localLevelManager = nullptr;
	Simple::EventManager* localEventManager = nullptr;
	Simple::PlaneReflection* localWaterPlane = nullptr;

	float localWaterHeight = 0;
}

void Impl::SimpleWorldGameWorld::SetGameWorld(Simple::GameWorld* aGameWorld)
{
	if (localGameWorld != nullptr)
		assert(false && "localGameWorld is already set. Is this a mistake?");

	localGameWorld = aGameWorld;
}

void Impl::SimpleWorldEventManager::SetEventManager(Simple::EventManager* aLevelManager)
{
	if (localEventManager != nullptr)
		assert(false && "localEventManager is already set. Is this a mistake?");

	localEventManager = aLevelManager;
}

void Impl::SimpleWorldLevelManager::SetLevelManager(Simple::LevelManager* aLevelManager)
{
	if (localLevelManager != nullptr)
		assert(false && "localLevelManager is already set. Is this a mistake?");

	localLevelManager = aLevelManager;
}

void SimpleWorld::SetActiveScene(const int aSceneIndex)
{
	localLevelManager->SetActiveScene(aSceneIndex);
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
	return localLevelManager->GetActiveSceneIndex();
}

std::shared_ptr<Simple::Scene> SimpleWorld::GetActiveScene()
{
	return localLevelManager->GetActiveScene();
}

Simple::PlaneReflection* SimpleWorld::GetWaterPlane()
{
	return localWaterPlane;
}

Simple::EventManager* SimpleWorld::GetEventmanager()
{
	return localEventManager;
}

Simple::Navmesh* SimpleWorld::GetNavmesh()
{
	return &localLevelManager->GetNavmesh();
}
