#include "Game/Precomplier/stdafx.h"
#include "Game/Managers/ImGuiManager/Tools/SceneTool.hpp"

namespace Tool
{
	SceneTool::SceneTool()
	{
	}

	void SceneTool::Draw()
	{
		Simple::GraphicsEngine* graphicsEngine = SimpleGlobal::GetGraphicsEngine();

		ImVec2 size(1280, 720);
		ImGui::SetNextWindowSize(size);
		bool test = true;
		if (ImGui::Begin("Render Test", &test, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize))
		{
			const ImTextureID textureID = graphicsEngine->GetWaterRefractionShaderResourceView().Get();
			ImGui::Image(textureID, size);
		}
		ImGui::End();
	}
}