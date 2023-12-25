#include "Game/stdafx.h"
#include "Engine/Graphics/Camera/Camera.hpp"
#include "Game/Managers/ImGuiManager/Tools/SettingsTool.hpp"

SettingsTool::SettingsTool()
	: mySelectedWindowSize(0)
	, mySelectedResolution(1)
	, mySelectedRasterizerState(0)
{
}

void SettingsTool::Draw()
{
	GraphicsEngine* graphicsEngine = SimpleGlobal::GetGraphicsEngine();

	if (ImGui::Begin("Settings"))
	{
		std::string fps = "FPS: " + std::to_string(SimpleGlobal::GetFPS());
		ImGui::Text(fps.c_str());

		ImGui::SameLine();
		ImGui::Dummy(ImVec2(50, 0));
		ImGui::SameLine();

		bool vsync = graphicsEngine->IsVSyncActive();
		if (ImGui::Checkbox("VSync", &vsync))
		{
			graphicsEngine->SetVSync(vsync);
		}


		ImGui::SameLine();
		ImGui::Dummy(ImVec2(50, 0));
		ImGui::SameLine();

		std::string drawCalls = "DrawCalls: " + std::to_string(SimpleGlobal::GetDrawCalls());
		ImGui::Text(drawCalls.c_str());

		ImGui::SetNextItemWidth(200);
		std::array<const char*, static_cast<int>(eRasterizerState::Count)> rasterizerStates;
		rasterizerStates[static_cast<int>(eRasterizerState::BackfaceCulling)] = "BackfaceCulling";
		rasterizerStates[static_cast<int>(eRasterizerState::NoFaceCulling)] = "NoFaceCulling";
		rasterizerStates[static_cast<int>(eRasterizerState::Wireframe)] = "Wireframe";
		rasterizerStates[static_cast<int>(eRasterizerState::WireframeNoCulling)] = "WireframeNoCulling";

		if (ImGui::Combo("RasterizerState", &mySelectedRasterizerState, rasterizerStates.data(), static_cast<int>(rasterizerStates.size())))
		{
			SimpleGlobal::GetGraphicsEngine()->SetRasterizerState(static_cast<eRasterizerState>(mySelectedRasterizerState));
		}

		ImGui::SetNextItemWidth(200.0f);
		std::vector<SimpleUtilities::Vector2i> resolutions =
		{
			SimpleUtilities::Vector2i(800, 600),
			SimpleUtilities::Vector2i(1280, 720),
			SimpleUtilities::Vector2i(1920, 1080),
		};

		const char* resolutionText[] = { "800x600", "1280x720", "1920x1080" };
		if (ImGui::Combo("Resolution", &mySelectedResolution, resolutionText, 3))
		{
			SimpleUtilities::Vector2f resolution(static_cast<float>(resolutions[mySelectedResolution].x), static_cast<float>(resolutions[mySelectedResolution].y));
			graphicsEngine->GetCamera()->SetResolution(resolution);
		}

		ImGui::SetNextItemWidth(200);
		std::vector<const char*> windowSizes;

		const std::string size1 = std::to_string(SimpleGlobal::GetWindowSize().x) + "x" + std::to_string(SimpleGlobal::GetWindowSize().y);
		windowSizes.push_back(size1.c_str());

		if (ImGui::Combo("WindowSize", &mySelectedWindowSize, windowSizes.data(), static_cast<int>(windowSizes.size())))
		{
			//TO-DO: Implement change window size
		}
	}

	ImGui::End();
}
