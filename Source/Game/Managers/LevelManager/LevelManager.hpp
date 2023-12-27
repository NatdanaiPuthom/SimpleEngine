#pragma once

class Scene;

class LevelManager final
{
public:
	LevelManager();
	~LevelManager();

	void Init();
	void Update();
	void Render();
public:
	void SetActiveScene(const int aSceneIndex);
public:
	std::shared_ptr<Scene> GetActiveScene();
private:
	std::shared_ptr<Scene> myActiveScene;
	std::unordered_map<int, std::shared_ptr<Scene>> myScenes;
};