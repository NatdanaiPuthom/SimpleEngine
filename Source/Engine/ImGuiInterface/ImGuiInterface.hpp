#pragma once

class ImGuiInterface
{
public:
	ImGuiInterface();
	~ImGuiInterface();

	void Init();
	void BeginFrame();
	void EndFrame();
};
