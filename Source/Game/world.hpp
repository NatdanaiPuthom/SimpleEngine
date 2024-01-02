#pragma once

namespace Simple
{
	class Scene;
	class PlaneReflection;
}

namespace SimpleWorld
{
	void SetActiveScene(const int aSceneIndex);
	void SetWaterHeight(const float aHeight);
	void SetWaterPlane(Simple::PlaneReflection* aWaterPlane);

	Simple::PlaneReflection* GetWaterPlane();

	std::shared_ptr<Simple::Scene> GetActiveScene();

	float GetWaterHeight();
	int GetActiveSceneIndex();
}
