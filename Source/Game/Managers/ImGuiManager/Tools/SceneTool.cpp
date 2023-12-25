#include "Game/stdafx.h"
#include "Game/Managers/ImGuiManager/Tools/SceneTool.hpp"

SceneTool::SceneTool()
{
}

void SceneTool::Draw()
{
	GraphicsEngine* graphicsEngine = SimpleGlobal::GetGraphicsEngine();

	if (ImGui::Begin("Scene"))
	{
		ImVec2 size(1280, 720);
		const ImTextureID textureID = graphicsEngine->GetWaterShaderResourceView().Get();
		ImGui::Image(textureID, size);
	}
	ImGui::End();
}
