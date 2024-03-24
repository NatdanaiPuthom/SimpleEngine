#include "Game/Precomplied/GamePch.hpp"
#include "Game/world.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"
#include "Game/GameWorld.hpp"
#include "Game/Managers/LevelManager/LevelManager.hpp"
#include "Game/Managers/EventManager/EventManager.hpp"
#include "Game/Managers/LevelManager/Template/Scene.hpp"
#include "Game/Test/ECS/ComponentManager.hpp"
#include <cassert>

namespace
{
	Simple::GameWorld* localGameWorld = nullptr;
	Simple::LevelManager* localLevelManager = nullptr;
	Simple::EventManager* localEventManager = nullptr;
	Simple::PlaneReflection* localWaterPlane = nullptr;

	ECS::ComponentManager* localComponentManager = nullptr;

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

void Impl::SimpleWorldComponentManager::SetComponentManager(ECS::ComponentManager* aComponentManager)
{
	localComponentManager = aComponentManager;
}

void World::SetActiveScene(const int aSceneIndex)
{
	localLevelManager->SetActiveScene(aSceneIndex);
}

void World::SetWaterHeight(const float aHeight)
{
	localWaterHeight = aHeight;
}

void World::SetWaterPlane(Simple::PlaneReflection* aWaterPlane)
{
	localWaterPlane = aWaterPlane;
}

float World::GetWaterHeight()
{
	return localWaterHeight;
}

int World::GetActiveSceneIndex()
{
	return localLevelManager->GetActiveSceneIndex();
}

std::shared_ptr<Simple::Scene> World::GetActiveScene()
{
	return localLevelManager->GetActiveScene();
}

Simple::PlaneReflection* World::GetWaterPlane()
{
	return localWaterPlane;
}

Simple::EventManager* World::GetEventmanager()
{
	return localEventManager;
}

Simple::Navmesh* World::GetNavmesh()
{
	return &localLevelManager->GetNavmesh();
}

ECS::ComponentManager* World::GetComponentManager()
{
	return localComponentManager;
}