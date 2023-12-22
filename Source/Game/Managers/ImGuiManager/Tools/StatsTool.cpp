#include "Game/stdafx.h"
#include "Game/Managers/ImGuiManager/Tools/StatsTool.hpp"

StatsTool::StatsTool()
{
}

void StatsTool::Draw()
{
	GraphicsEngine* graphicsEngine = SimpleGlobal::GetGraphicsEngine();

	if (ImGui::Begin("Frame Counter"))
	{
		std::string fps = "FPS: " + std::to_string(SimpleGlobal::GetFPS());
		ImGui::Text(fps.c_str());

		bool vsync = graphicsEngine->IsVSyncActive();
		if (ImGui::Checkbox("VSync", &vsync))
		{
			graphicsEngine->SetVSync(vsync);
		}
	}

	ImGui::End();
}
