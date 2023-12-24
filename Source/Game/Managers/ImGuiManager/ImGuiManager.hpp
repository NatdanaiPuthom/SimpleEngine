#pragma once

class ToolInterface;

class ImGuiManager final
{
public:
	ImGuiManager();
	~ImGuiManager();

	void AddTool(std::unique_ptr<ToolInterface> aTool);
	void Render();
private:
	std::vector<std::unique_ptr<ToolInterface>> myTools;
};
