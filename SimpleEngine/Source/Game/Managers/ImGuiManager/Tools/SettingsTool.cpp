#include "Game/Precomplied/GamePch.hpp"
#include "Game/Managers/ImGuiManager/Tools/SettingsTool.hpp"

namespace Tool
{
	SettingsTool::SettingsTool()
		: mySelectedWindowSize(1)
		, mySelectedResolution(1)
		, mySelectedRasterizerState(0)
		, myActiveSceneIndex(0)
		, myConsoleIsOpen(true)
	{
	}

	void SettingsTool::Init()
	{
		LoadDataFromJson();
		UpdateAndFetchCurrentMonitorResolution();

		myWindowSizes.push_back(Math::Vector2ui(800, 600));
		myWindowSizes.push_back(Math::Vector2ui(1280, 720));
		myWindowSizes.push_back(Math::Vector2ui(1600, 900));
		myWindowSizes.push_back(Math::Vector2ui(1920, 1080));

		if (myWindowSizes.back().x < myMonitorResolution.x &&
			myWindowSizes.back().y < myMonitorResolution.y)
		{
			myWindowSizes.push_back(myMonitorResolution);
		}
	}

	void SettingsTool::Draw()
	{
		if (ImGui::Begin("Settings"))
		{
			Simple::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

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

			ImGui::Dummy(ImVec2(0, 20));
			ToggleConsole();
			ToggleRenderBoundingBox();

			ImGui::Dummy(ImVec2(0, 20));
			AdjustActiveScene();
		}

		ImGui::End();
	}

	void SettingsTool::UpdateAndFetchCurrentMonitorResolution()
	{
		HMONITOR hMonitor = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY);

		MONITORINFOEX monitorInfo = { sizeof(MONITORINFOEX) };
		GetMonitorInfo(hMonitor, &monitorInfo);

		myMonitorResolution.x = static_cast<unsigned int>(monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left);
		myMonitorResolution.y = static_cast<unsigned int>(monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top);
	}

	void SettingsTool::LoadDataFromJson()
	{
		const std::string filename = SimpleUtilities::GetAbsolutePath(SIMPLE_LEVELS_FILENAME);
		std::ifstream file(filename);
		assert(file.is_open() && "Failed To Open File");

		const nlohmann::json json = nlohmann::json::parse(file);
		file.close();

		const nlohmann::json& scenesIndexes = json["scenes"];

		myScenes.resize(scenesIndexes.size());
		for (size_t i = 0; i < scenesIndexes.size(); ++i)
		{
			const std::string name = scenesIndexes[i]["name"];
			myScenes[static_cast<size_t>(scenesIndexes[i]["id"])] = name;
		}
	}

	void SettingsTool::ToggleConsole()
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

	void SettingsTool::ToggleVSync(Simple::GraphicsEngine* aGraphicsEngine)
	{
		bool vsync = aGraphicsEngine->IsVSyncActive();

		if (ImGui::Checkbox("VSync", &vsync))
		{
			aGraphicsEngine->SetVSync(vsync);
		}
	}

	void SettingsTool::ToggleRenderBoundingBox()
	{
		auto renderer = Global::GetRenderer();

		bool debugMode = renderer->IsDebugModeOn();
		if (ImGui::Checkbox("Draw BoundingBox", &debugMode))
		{
			renderer->SetDebugMode(debugMode);
		}
	}

	void SettingsTool::ShowDrawCalls()
	{
		std::string drawCalls = "DrawCalls: " + std::to_string(Global::GetDrawCalls());
		ImGui::Text(drawCalls.c_str());
	}

	void SettingsTool::ShowFPS()
	{
		std::string fps = "FPS: " + std::to_string(Global::GetFPS());
		ImGui::Text(fps.c_str());
	}

	void SettingsTool::AdjustWindowSize()
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
			const auto currentWindowSize = Global::GetWindowSize();

			if (currentWindowSize.x != myWindowSizes[mySelectedWindowSize].x &&
				currentWindowSize.y != myWindowSizes[mySelectedWindowSize].y)
			{
				UpdateAndFetchCurrentMonitorResolution();

				if (myWindowSizes[mySelectedWindowSize].x == myMonitorResolution.x &&
					myWindowSizes[mySelectedWindowSize].y == myMonitorResolution.y)
				{
					Global::SetWindowSize(myMonitorResolution, true);
				}
				else
				{
					Global::SetWindowSize(myWindowSizes[mySelectedWindowSize]);
				}
			}
		}
	}

	void SettingsTool::AdjustResolution()
	{
		ImGui::SetNextItemWidth(200.0f);

		std::vector<Math::Vector2ui> resolutions =
		{
			Math::Vector2ui(800, 600),
			Math::Vector2ui(1280, 720),
			Math::Vector2ui(1920, 1080),
		};

		const char* resolutionText[] = { "800x600", "1280x720", "1920x1080" };
		if (ImGui::Combo("Resolution", &mySelectedResolution, resolutionText, 3))
		{
			Global::SetResolution(resolutions[mySelectedResolution]);
		}
	}

	void SettingsTool::AdjustRasterizerState()
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
			Global::GetGraphicsEngine()->SetRasterizerState(static_cast<eRasterizerState>(mySelectedRasterizerState));
		}
	}

	void SettingsTool::AdjustActiveScene()
	{
		std::vector<const char*> sceneNameChar;
		sceneNameChar.reserve(myScenes.size());

		for (size_t i = 0; i < myScenes.size(); ++i)
		{
			sceneNameChar.push_back(myScenes[i].c_str());
		}

		ImGui::SetNextItemWidth(200);
		myActiveSceneIndex = World::GetActiveSceneIndex();
		if (ImGui::Combo("Active Scene", &myActiveSceneIndex, sceneNameChar.data(), static_cast<int>(sceneNameChar.size())))
		{
			World::SetActiveScene(myActiveSceneIndex);
		}
	}

	void SettingsTool::AdjustFPSCap(Simple::GraphicsEngine* aGraphicsEngine)
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
		
		for (int i = 1; i <= 4; ++i)
		{
			const int hz = monitorUpdateFrequency / i;

			if (hz >= 60)
			{
				fpsCapAsString.push_back(std::to_string(hz));
			}
		}

		std::vector<const char*> fpsCapAsConstChar;
		fpsCapAsConstChar.reserve(fpsCapAsString.size());
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