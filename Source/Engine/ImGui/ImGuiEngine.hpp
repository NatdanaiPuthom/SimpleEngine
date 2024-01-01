#pragma once

class ImGuiEngine final
{
public:
	ImGuiEngine();
	~ImGuiEngine();

	void Init();
	void BeginFrame();
	void EndFrame();
};
