#pragma once

class ImGuiManager
{
public:
	ImGuiManager();
	~ImGuiManager();

	void Render();
private:
	int mySelectedResolution;
};
