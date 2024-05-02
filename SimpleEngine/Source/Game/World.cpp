#include "Game/Precomplied/GamePch.hpp"
#include "Game/world.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"
#include "Game/Managers/LevelManager/LevelManager.hpp"
#include <cassert>

namespace World
{
	Simple::LevelManager* localLevelManager = nullptr;
	Simple::EventManager* localEventManager = nullptr;
	Simple::PlaneReflection* localWaterPlane = nullptr;
	ECS::EntityComponentSystem* localECS = nullptr;

	float localWaterHeight = 0;
}

namespace World
{
	void SetActiveScene(const int aSceneIndex)
	{
		localLevelManager->SetActiveScene(aSceneIndex);
	}

	void SetWaterHeight(const float aHeight)
	{
		localWaterHeight = aHeight;
	}

	void SetWaterPlane(Simple::PlaneReflection* aWaterPlane)
	{
		localWaterPlane = aWaterPlane;
	}

	float GetWaterHeight()
	{
		return localWaterHeight;
	}

	int GetActiveSceneIndex()
	{
		return localLevelManager->GetActiveSceneIndex();
	}

	std::shared_ptr<Simple::Scene> GetActiveScene()
	{
		return localLevelManager->GetActiveScene();
	}

	Simple::PlaneReflection* GetWaterPlane()
	{
		return localWaterPlane;
	}

	Simple::EventManager* GetEventmanager()
	{
		return localEventManager;
	}

	Simple::Navmesh* GetNavmesh()
	{
		return &localLevelManager->GetNavmesh();
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

	void SimpleWorldLevelManager::SetLevelManager(Simple::LevelManager* aLevelManager)
	{
		if (World::localLevelManager != nullptr)
			assert(false && "localLevelManager is already set. Is this a mistake?");

		World::localLevelManager = aLevelManager;
	}

	void SimpleWorldECS::SetECS(ECS::EntityComponentSystem* aECS)
	{
		if (World::localECS != nullptr)
			assert(false && "localLevelManager is already set. Is this a mistake?");

		World::localECS = aECS;
	}
}