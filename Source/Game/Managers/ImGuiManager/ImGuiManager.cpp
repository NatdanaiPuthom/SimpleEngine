#include "stdafx.h"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"
#include "Engine/Graphics/GraphicsEngine.hpp"
#include "Engine/Graphics/Camera/Camera.hpp"
#include <External/imgui.h>

ImGuiManager::ImGuiManager()
{
}

ImGuiManager::~ImGuiManager()
{
}

void ImGuiManager::Render()
{
	GraphicsEngine* graphicsEngine = SimplyGlobal::GetGraphicsEngine();

	if (ImGui::Begin("Camera Controls")) //TO-DO: Move All ImGui stuff to it's own place
	{
		ImGui::Text("Movements    - WASD");
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(100, 0));
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100.0f);

		float speed = graphicsEngine->GetCamera()->GetMoveSpeed();
		if (ImGui::SliderFloat("MoveSpeed", &speed, 0, 100, "%0.2f"))
		{
			graphicsEngine->GetCamera()->SetMoveSpeed(speed);
		}

		ImGui::Text("FreeFly      - TAB");
		ImGui::Text("Speedx3      - Hold Shift");
		ImGui::Text("Fly Up       - SPACE");
		ImGui::Text("Fly Down     - Hold Ctrl + SPACE");
		ImGui::Text("Rotate Left  - E");
		ImGui::Text("Rotate Right - Q");
		ImGui::Text("Rotate Up    - C");
		ImGui::Text("Rotate Down  - Z");
	}
	ImGui::End();

	if (ImGui::Begin("Frame Counter"))
	{
		std::string fps = "FPS: " + std::to_string(SimplyGlobal::GetFPS());
		ImGui::Text(fps.c_str());

		bool vsync = graphicsEngine->IsVSyncActive();
		if (ImGui::Checkbox("VSync", &vsync))
		{
			graphicsEngine->SetVSync(vsync);
		}
	}
	ImGui::End();

	if (ImGui::Begin("Lights"))
	{
		if (ImGui::BeginTabBar("Light Tab Bar"))
		{
			if (ImGui::BeginTabItem("Directional Light"))
			{
				ImGui::SetNextItemWidth(400.0f);
				SimpleUtilities::Vector3f direction = graphicsEngine->GetDirectionalLightDirection();
				if (ImGui::SliderFloat3("Direction", &direction.x, -1.0f, 1.0f, "%.03f"))
				{
					graphicsEngine->SetDirectionalLightDirection(direction);
				}

				ImGui::SetNextItemWidth(400.0f);
				SimpleUtilities::Vector3f color = graphicsEngine->GetDirectionalLightColor();
				if (ImGui::SliderFloat3("Color", &color.x, 0.0f, 1.0f, "%.03f"))
				{
					graphicsEngine->SetDirectionalLightColor(color);
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Ambient Light"))
			{
				ImGui::SetNextItemWidth(400.0f);
				SimpleUtilities::Vector3f skyColor = graphicsEngine->GetSkyColor();
				if (ImGui::SliderFloat3("SkyColor", &skyColor.x, 0.0f, 1.0f, "%0.3f"))
				{
					graphicsEngine->SetSkyColor(skyColor);
				}

				ImGui::SetNextItemWidth(400.0f);
				SimpleUtilities::Vector3f groundColor = graphicsEngine->GetGroundColor();
				if (ImGui::SliderFloat3("GroundColor", &groundColor.x, 0.0f, 1.0f, "%0.3f"))
				{
					graphicsEngine->SetGroundColor(groundColor);
				}
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}
	ImGui::End();

	/*if (ImGui::Begin("Scene")) //Disabled due to issue with ImGui::Image rendering
	{
		ImVec2 size(1280, 720);
		const ImTextureID textureID = graphicsEngine->GetShaderResourceView().Get();
		ImGui::Image(textureID, size);
	}
	ImGui::End();*/
}
