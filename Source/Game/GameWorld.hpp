#pragma once

class ImGuiManager;

class GameWorld final
{
public:
	GameWorld();
	~GameWorld();

	void Init();
	void Update();
	void Render();
	void RenderImGui();
private:
	std::unique_ptr<ImGuiManager> myImGuiManager;
	std::vector<std::unique_ptr<ModelInstance>> myModelInstances;
	std::unique_ptr<ModelInstance> myDirectionalLight;
};
