#pragma once

namespace Simple
{
	class Scene;
}

class Model;

namespace SimpleWorld
{
	void SetActiveScene(const int aSceneIndex);
	void SetWaterHeight(const float aHeight);
	void SetWaterPlane(std::shared_ptr<Model> aWaterPlane);

	std::shared_ptr<Simple::Scene> GetActiveScene();
	std::shared_ptr<Model> GetWaterPlane();

	float GetWaterHeight();
	int GetActiveSceneIndex();
}
