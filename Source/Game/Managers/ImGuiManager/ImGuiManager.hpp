#pragma once

class ToolInterface;
class Renderer;

class ImGuiManager final
{
public:
	ImGuiManager();
	~ImGuiManager();


	void AddTool(std::unique_ptr<ToolInterface> aTool);
	void Render();
public:
	void SetRenderer(Renderer* aRenderer);
private:
	std::vector<std::unique_ptr<ToolInterface>> myTools;

	Renderer* myRenderer;
};
