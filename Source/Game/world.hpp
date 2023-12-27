#pragma once

class Scene;

namespace SimpleWorld
{
	void SetActiveScene(const int aSceneIndex);

	std::shared_ptr<Scene> GetActiveScene();
}
