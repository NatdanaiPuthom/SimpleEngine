#pragma once

class ImGuiEngine
{
public:
	ImGuiEngine();
	~ImGuiEngine();

	void Init();
	void BeginFrame();
	void EndFrame();
};
