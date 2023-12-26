#pragma once

class ImGuiManager;
class PlaneReflection;

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
	std::vector<std::unique_ptr<ModelInstance>> myModelInstances;

	std::unique_ptr<PlaneReflection> myPlaneReflection;
};
