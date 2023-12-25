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

		{ //VSync
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(50, 0));
			ImGui::SameLine();

			bool vsync = graphicsEngine->IsVSyncActive();
			if (ImGui::Checkbox("VSync", &vsync))
			{
				graphicsEngine->SetVSync(vsync);
			}
		}

		{ //DrawCalls
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(50, 0));
			ImGui::SameLine();

			std::string drawCalls = "DrawCalls: " + std::to_string(SimpleGlobal::GetDrawCalls());
			ImGui::Text(drawCalls.c_str());
		}

		{ //FPS Cap
			int monitorUpdateFrequency = 0;

			{
				HDC hdc = GetDC(0);
				DEVMODE devMode;
				devMode.dmSize = sizeof(DEVMODE);
				EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devMode);

				monitorUpdateFrequency = static_cast<int>(devMode.dmDisplayFrequency);
				ReleaseDC(0, hdc);
			}

			std::vector<std::string> fpsCapAsString;
			fpsCapAsString.reserve(5);
			fpsCapAsString.push_back("Uncapped");
			fpsCapAsString.push_back(std::to_string(monitorUpdateFrequency));
			fpsCapAsString.push_back(std::to_string(monitorUpdateFrequency / 2));
			fpsCapAsString.push_back(std::to_string(monitorUpdateFrequency / 3));
			fpsCapAsString.push_back(std::to_string(monitorUpdateFrequency / 4));

			std::vector<const char*> fpsCapAsConstChar;
			fpsCapAsConstChar.reserve(5);
			for (const std::string& fpsString : fpsCapAsString)
			{
				fpsCapAsConstChar.push_back(fpsString.c_str());
			}

			int selectedFPSLevelCap = graphicsEngine->GetFPSLevelCap();
			if (selectedFPSLevelCap == 1)
			{
				ImGui::Text("FPS Capped: %s", fpsCapAsConstChar[selectedFPSLevelCap]);
			}
			else
			{
				ImGui::SetNextItemWidth(200);
				if (ImGui::Combo("FPS Cap", &selectedFPSLevelCap, fpsCapAsConstChar.data(), static_cast<int>(fpsCapAsConstChar.size())))
				{
					if (selectedFPSLevelCap == 1)
						graphicsEngine->SetVSync(true);
					else
						graphicsEngine->SetFPSLevelCap(selectedFPSLevelCap);
				}
			}
		}

		{ //RasterizeState
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
		}

		{ //Resolutions
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
		}

		{ //Window Sizes	
			ImGui::SetNextItemWidth(200);
			std::vector<const char*> windowSizes;

			const std::string size1 = std::to_string(SimpleGlobal::GetWindowSize().x) + "x" + std::to_string(SimpleGlobal::GetWindowSize().y);
			windowSizes.push_back(size1.c_str());

			if (ImGui::Combo("WindowSize", &mySelectedWindowSize, windowSizes.data(), static_cast<int>(windowSizes.size())))
			{
				//TO-DO: Implement change window size
			}
		}
	}
	ImGui::End();
}