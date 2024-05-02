#pragma once
#include "Engine/ECS/ECS.hpp"
#include "Graphics/Model/Special/PlaneReflection.h"
#include "Game/Managers/EventManager/EventManager.hpp"
#include "Game/Managers/LevelManager/Template/Scene.hpp"
#include "Game/Navmesh/Navmesh.hpp"
#include <memory>

namespace World
{
	void SetActiveScene(const int aSceneIndex);
	void SetWaterHeight(const float aHeight);
	void SetWaterPlane(Simple::PlaneReflection* aWaterPlane);

	Simple::PlaneReflection* GetWaterPlane();
	Simple::EventManager* GetEventmanager();
	Simple::Navmesh* GetNavmesh();
	Simple::ECS* GetECS();

	std::shared_ptr<Simple::Scene> GetActiveScene();

	float GetWaterHeight();
	int GetActiveSceneIndex();
}