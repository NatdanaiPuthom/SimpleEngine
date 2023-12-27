#pragma once

class Scene;

namespace SimpleWorld
{
	void SetActiveScene(const int aSceneIndex);

	int GetActiveSceneIndex();
	std::shared_ptr<Scene> GetActiveScene();
}
