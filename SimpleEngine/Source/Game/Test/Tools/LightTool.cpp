#include "Game/Precomplied/GamePch.hpp"
#include "Game/Test/Tools/LightTool.hpp"

namespace Tool
{
	LightTool::LightTool()
	{
	}

	void LightTool::Draw()
	{
		Simple::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

		if (ImGui::Begin("Lights"))
		{
			auto& directionalLight = World::GetActiveScene()->myDirectionalLight;

			if (ImGui::BeginTabBar("Light Tab Bar"))
			{
				if (ImGui::BeginTabItem("Directional Light"))
				{
					ImGui::SetNextItemWidth(400.0f);
					Math::Vector3f position = directionalLight->myModel->GetPosition();
					if (ImGui::DragFloat3("Position", &position.x, 0.1f))
					{
						directionalLight->myModel->SetPosition(position);
					}

					ImGui::SetNextItemWidth(400.0f);
					Math::Vector3f direction = graphicsEngine->GetDirectionalLightDirection();
					if (ImGui::SliderFloat3("Direction", &direction.x, -1.0f, 1.0f, "%.03f"))
					{
						graphicsEngine->SetDirectionalLightDirection(direction);
					}

					ImGui::SetNextItemWidth(400.0f);
					Math::Vector4f color = graphicsEngine->GetDirectionalLightColor();
					if (ImGui::SliderFloat3("Color", &color.x, 0.0f, 1.f, "%.03f"))
					{
						graphicsEngine->SetDirectionalLightColor(color);
					}

					ImGui::SameLine();
					ImGui::Dummy(ImVec2(50, 0));
					ImGui::SameLine();
					ImGui::SetNextItemWidth(100.0f);
					if (ImGui::SliderFloat("Intensity", &color.w, 0.0f, 10.f, "%.03f"))
					{
						graphicsEngine->SetDirectionalLightColor(color);
					}

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Ambient Light"))
				{
					ImGui::SetNextItemWidth(400.0f);
					Math::Vector4f skyColor = graphicsEngine->GetSkyColor();
					if (ImGui::SliderFloat4("SkyColor", &skyColor.x, 0.0f, 1.0f, "%0.3f"))
					{
						graphicsEngine->SetSkyColor(skyColor);
					}

					ImGui::SetNextItemWidth(400.0f);
					Math::Vector4f groundColor = graphicsEngine->GetGroundColor();
					if (ImGui::SliderFloat4("GroundColor", &groundColor.x, 0.0f, 1.0f, "%0.3f"))
					{
						graphicsEngine->SetGroundColor(groundColor);
					}
					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}
		}
		ImGui::End();
	}
}