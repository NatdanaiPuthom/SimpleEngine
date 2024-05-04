#pragma once
#include "Engine/ECS/ECS.hpp"
#include "Game/Managers/EventManager/EventManager.hpp"
#include "Game/Managers/LevelManager/Template/Scene.hpp"
#include "Game/Navmesh/Navmesh.hpp"
#include <memory>

namespace World
{
	void SetActiveScene(const int aSceneIndex);

	Simple::EventManager* GetEventmanager();
	Simple::Navmesh* GetNavmesh();
	ECS::EntityComponentSystem* GetECS();

	std::shared_ptr<Simple::Scene> GetActiveScene();

	int GetActiveSceneIndex();
}