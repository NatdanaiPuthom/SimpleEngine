#include "Game/Precomplier/stdafx.h"
#include "Game/Managers/ImGuiManager/Tools/Scene.hpp"

namespace Tool
{
	Scene::Scene()
	{
	}

	void Scene::Draw()
	{
		Simple::GraphicsEngine* graphicsEngine = SimpleGlobal::GetGraphicsEngine();

		if (ImGui::Begin("Render Test"))
		{
			ImVec2 size(1280, 720);
			const ImTextureID textureID = graphicsEngine->GetWaterRefractionShaderResourceView().Get();
			ImGui::Image(textureID, size);
		}
		ImGui::End();
	}
}