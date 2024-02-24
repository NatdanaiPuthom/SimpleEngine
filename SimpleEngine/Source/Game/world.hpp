#pragma once
#include "Graphics/Model/Special/PlaneReflection.h"
#include "Game/Managers/EventManager/EventManager.hpp"
#include "Game/Navmesh/Navmesh.hpp"
#include "Game/Managers/LevelManager/Template/Scene.hpp"
#include <memory>

namespace SimpleWorld
{
	void SetActiveScene(const int aSceneIndex);
	void SetWaterHeight(const float aHeight);
	void SetWaterPlane(Simple::PlaneReflection* aWaterPlane);

	Simple::PlaneReflection* GetWaterPlane();
	Simple::EventManager* GetEventmanager();
	Simple::Navmesh* GetNavmesh();

	std::shared_ptr<Simple::Scene> GetActiveScene();

	float GetWaterHeight();
	int GetActiveSceneIndex();
}
