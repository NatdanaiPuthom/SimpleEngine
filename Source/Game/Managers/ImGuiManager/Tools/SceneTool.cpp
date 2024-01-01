#include "Game/Precomplier/stdafx.h"
#include "Game/Managers/ImGuiManager/Tools/SceneTool.hpp"

namespace Tool
{
	Scene::Scene()
	{
	}

	void Scene::Draw()
	{
		Simple::GraphicsEngine* graphicsEngine = SimpleGlobal::GetGraphicsEngine();

		if (ImGui::Begin("Mirror Scene"))
		{
			ImVec2 size(1280, 720);
			const ImTextureID textureID = graphicsEngine->GetWaterShaderResourceView().Get();
			ImGui::Image(textureID, size);
		}
		ImGui::End();
	}
}