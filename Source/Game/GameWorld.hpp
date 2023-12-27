#pragma once

class ImGuiManager;
class PlaneReflection;
class Scene;

class GameWorld final
{
public:
	GameWorld();
	~GameWorld();

	void Init();
	void Update();
	void Render();

	void RenderImGui();
	void RenderReflection();
	void RenderUpSideDown();
public:
	void SetActiveScene(const int aSceneIndex);
public:
	std::shared_ptr<Scene> GetActiveScene();
private:
	std::shared_ptr<Scene> myActiveScene;
	std::unordered_map<int, std::shared_ptr<Scene>> myScenes;

	std::unique_ptr<ImGuiManager> myImGuiManager;
	std::unique_ptr<PlaneReflection> myPlaneReflection;
};
