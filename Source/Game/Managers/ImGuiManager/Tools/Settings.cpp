#include "Game/Precomplier/stdafx.h"
#include "Game/Managers/ImGuiManager/Tools/Settings.hpp"

namespace Tool
{
	Settings::Settings()
		: mySelectedWindowSize(1)
		, mySelectedResolution(1)
		, mySelectedRasterizerState(0)
		, myConsoleIsOpen(true)
	{
		UpdateAndFetchCurrentMonitorResolution();

		myWindowSizes.push_back(SimpleUtilities::Vector2ui(800, 600));
		myWindowSizes.push_back(SimpleUtilities::Vector2ui(1280, 720));
		myWindowSizes.push_back(SimpleUtilities::Vector2ui(1600, 900));
		myWindowSizes.push_back(SimpleUtilities::Vector2ui(1920, 1080));

		if (myWindowSizes.back().x < myMonitorResolution.x &&
			myWindowSizes.back().y < myMonitorResolution.y)
		{
			myWindowSizes.push_back(myMonitorResolution);
		}
	}

	void Settings::Draw()
	{
		if (ImGui::Begin("Settings"))
		{
			Simple::GraphicsEngine* graphicsEngine = SimpleGlobal::GetGraphicsEngine();

			ShowFPS();

			{
				ImGui::SameLine();
				ImGui::Dummy(ImVec2(50, 0));
				ImGui::SameLine();

				ToggleVSync(graphicsEngine);
			}

			{
				ImGui::SameLine();
				ImGui::Dummy(ImVec2(50, 0));
				ImGui::SameLine();

				ShowDrawCalls();
			}

			AdjustFPSCap(graphicsEngine);
			AdjustRasterizerState();
			AdjustResolution();
			AdjustWindowSize();
			ToggleConsole();
			ToggleMusic();
		}

		ImGui::End();
	}

	void Settings::UpdateAndFetchCurrentMonitorResolution()
	{
		HMONITOR hMonitor = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY);

		MONITORINFOEX monitorInfo = { sizeof(MONITORINFOEX) };
		GetMonitorInfo(hMonitor, &monitorInfo);

		myMonitorResolution.x = static_cast<unsigned int>(monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left);
		myMonitorResolution.y = static_cast<unsigned int>(monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top);
	}

	void Settings::ToggleConsole()
	{
		if (ImGui::Checkbox("Show Console", &myConsoleIsOpen))
		{
			HWND consoleWindow = GetConsoleWindow();

			if (myConsoleIsOpen)
				ShowWindow(consoleWindow, SW_SHOW);
			else
				ShowWindow(consoleWindow, SW_HIDE);
		}
	}

	void Settings::ToggleVSync(Simple::GraphicsEngine* aGraphicsEngine)
	{
		bool vsync = aGraphicsEngine->IsVSyncActive();

		if (ImGui::Checkbox("VSync", &vsync))
		{
			aGraphicsEngine->SetVSync(vsync);
		}
	}

	void Settings::ToggleMusic()
	{
		auto audioManager = SimpleGlobal::GetAudioManager();

		bool musicOn = audioManager->IsMusicMuted();
		if (ImGui::Checkbox("Mute Music", &musicOn))
		{
			audioManager->ToggleMuteMusic();
		}
	}

	void Settings::ShowDrawCalls()
	{
		std::string drawCalls = "DrawCalls: " + std::to_string(SimpleGlobal::GetDrawCalls());
		ImGui::Text(drawCalls.c_str());
	}

	void Settings::ShowFPS()
	{
		std::string fps = "FPS: " + std::to_string(SimpleGlobal::GetFPS());
		ImGui::Text(fps.c_str());
	}

	void Settings::AdjustWindowSize()
	{
		ImGui::SetNextItemWidth(200);

		std::vector<std::string> windowSizeAsString;
		for (const auto& size : myWindowSizes)
		{
			const std::string string = std::to_string(size.x) + "x" + std::to_string(size.y);
			windowSizeAsString.push_back(string);
		}

		std::vector<const char*> windowSizeAsChar;
		for (const std::string& string : windowSizeAsString)
		{
			windowSizeAsChar.push_back(string.c_str());
		}

		if (ImGui::Combo("WindowSize", &mySelectedWindowSize, windowSizeAsChar.data(), static_cast<int>(myWindowSizes.size())))
		{
			const auto currentWindowSize = SimpleGlobal::GetWindowSize();

			if (currentWindowSize.x != myWindowSizes[mySelectedWindowSize].x &&
				currentWindowSize.y != myWindowSizes[mySelectedWindowSize].y)
			{
				UpdateAndFetchCurrentMonitorResolution();

				if (myWindowSizes[mySelectedWindowSize].x == myMonitorResolution.x &&
					myWindowSizes[mySelectedWindowSize].y == myMonitorResolution.y)
				{
					SimpleGlobal::SetWindowSize(myMonitorResolution, true);
				}
				else
				{
					SimpleGlobal::SetWindowSize(myWindowSizes[mySelectedWindowSize]);
				}
			}
		}
	}

	void Settings::AdjustResolution()
	{
		ImGui::SetNextItemWidth(200.0f);

		std::vector<SimpleUtilities::Vector2ui> resolutions =
		{
			SimpleUtilities::Vector2ui(800, 600),
			SimpleUtilities::Vector2ui(1280, 720),
			SimpleUtilities::Vector2ui(1920, 1080),
		};

		const char* resolutionText[] = { "800x600", "1280x720", "1920x1080" };
		if (ImGui::Combo("Resolution", &mySelectedResolution, resolutionText, 3))
		{
			SimpleGlobal::SetResolution(resolutions[mySelectedResolution]);
		}
	}

	void Settings::AdjustRasterizerState()
	{
		ImGui::SetNextItemWidth(200);

		std::array<const char*, static_cast<int>(eRasterizerState::Count)> rasterizerStates = {};
		rasterizerStates[static_cast<int>(eRasterizerState::BackfaceCulling)] = "BackfaceCulling";
		rasterizerStates[static_cast<int>(eRasterizerState::NoFaceCulling)] = "NoFaceCulling";
		rasterizerStates[static_cast<int>(eRasterizerState::Wireframe)] = "Wireframe";
		rasterizerStates[static_cast<int>(eRasterizerState::WireframeNoCulling)] = "WireframeNoCulling";
		rasterizerStates[static_cast<int>(eRasterizerState::FrontFaceCulling)] = "FrontFaceCulling";

		if (ImGui::Combo("RasterizerState", &mySelectedRasterizerState, rasterizerStates.data(), static_cast<int>(rasterizerStates.size())))
		{
			SimpleGlobal::GetGraphicsEngine()->SetRasterizerState(static_cast<eRasterizerState>(mySelectedRasterizerState));
		}
	}

	void Settings::AdjustFPSCap(Simple::GraphicsEngine* aGraphicsEngine)
	{
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

		int selectedFPSLevelCap = aGraphicsEngine->GetFPSLevelCap();
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
					aGraphicsEngine->SetVSync(true);
				else
					aGraphicsEngine->SetFPSLevelCap(selectedFPSLevelCap);
			}
		}
	}
}