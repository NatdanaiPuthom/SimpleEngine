#pragma once

class ImGuiManager;
class PlaneReflection;
class LevelManager;

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
private:
	std::unique_ptr<ImGuiManager> myImGuiManager;
	std::unique_ptr<PlaneReflection> myPlaneReflection;
	std::unique_ptr<LevelManager> myLevelManager;

	std::unique_ptr<Drawer::Line> myLine;
};
