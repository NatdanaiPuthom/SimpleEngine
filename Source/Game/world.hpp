#pragma once

namespace Simple
{
	class Scene;
	class PlaneReflection;
	class EventManager;
	class Navmesh;
}

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
