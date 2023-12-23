#pragma once

class Mesh;
class Renderer;
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
	std::unique_ptr<Renderer> myRenderer;
	std::unique_ptr<ImGuiManager> myImGuiManager;
};
