#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Tools/SettingsTool.hpp"
#include "Editor/Editor.hpp"
#include "Engine/ImGui/ImGuiEngine.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace Editor
{
	SettingsTool::SettingsTool()
		: mySelectedWindowSize(0)
		, mySelectedRasterizerState(0)
		, myActiveSceneIndex(0)
		, myConsoleIsOpen(true)
		, myMusicIsActive(true)
	{
	}

	void SettingsTool::Init()
	{
		LoadDataFromJson();
		UpdateAndFetchCurrentMonitorResolution();

		myWindowSizes.push_back(Math::Vector2ui(1280, 720));
		myWindowSizes.push_back(Math::Vector2ui(1600, 900));
		myWindowSizes.push_back(Math::Vector2ui(1920, 1080));

		if (myWindowSizes.back().x < myMonitorResolution.x &&
			myWindowSizes.back().y < myMonitorResolution.y)
		{
			myWindowSizes.push_back(myMonitorResolution);
		}

		if (myMusicIsActive)
		{
			MainSingleton::GetAudioManager().PlayMusic("StardewValley.mp3");
		}
	}

	void SettingsTool::Update()
	{
		Math::Vector2ui resolution = Global::GetResolution();

		if (myWindowSizes[mySelectedWindowSize].x != resolution.x && myWindowSizes[mySelectedWindowSize].y != resolution.y)
		{
			for (size_t i = 0; i < myWindowSizes.size(); ++i)
			{
				if (myWindowSizes[i].x == resolution.x && myWindowSizes[i].y == resolution.y)
				{
					mySelectedWindowSize = static_cast<int>(i);
					break;
				}
			}
		}
	}

	void SettingsTool::Draw()
	{
		if (ImGui::Begin("Settings"))
		{
			Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

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
			AdjustWindowSize();

			ImGui::Separator();

			ImGui::Dummy(ImVec2(0, 20));
			ToggleConsole();

			if (ImGui::Checkbox("Render Debug Lines", &EditorEngine::myStaticShouldRenderDebugLines))
			{
			}

			ImGui::Separator();
			ImGui::Dummy(ImVec2(0, 20));

			Simpleton::AudioManager& audioManager = MainSingleton::GetAudioManager();

			const std::string musicName = "StardewValley.mp3";

			if (ImGui::Checkbox("Play Music", &myMusicIsActive))
			{
				if (myMusicIsActive == true)
				{
					audioManager.PlayMusic(musicName);
				}
				else
				{
					audioManager.StopAllMusic();
				}
			}

			const std::string musicNameAsText = "Music: " + musicName;
			ImGui::Text(musicNameAsText.c_str());

			float musicVolume = audioManager.GetMusicVolume();
			if (ImGui::DragFloat("Music Volume", &musicVolume, 0.01f, 0.0f, 1.0f))
			{
				audioManager.ChangeMusicVolume(musicVolume);
			}

			ImGui::Separator();

			ImGui::Dummy(ImVec2(0, 20));
			AdjustActiveScene();
			AdjustEditorStyle();
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
		const std::string levelJsonFileName = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_LEVELS);

		std::ifstream levelFile(levelJsonFileName);
		assert(levelFile.is_open() && "Failed To Open file");

		const nlohmann::json levelJson = nlohmann::json::parse(levelFile);
		levelFile.close();

		const nlohmann::json& scenesIndexes = levelJson["scenes"];

		myScenes.resize(scenesIndexes.size());
		for (size_t i = 0; i < scenesIndexes.size(); ++i)
		{
			const std::string name = scenesIndexes[i]["name"];
			myScenes[static_cast<size_t>(scenesIndexes[i]["id"])] = name;
		}

		const std::string editorJsonFileName = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_EDITOR);
		std::ifstream editorFile(editorJsonFileName);
		assert(editorFile.is_open() && "Failed To Open file");

		const nlohmann::json editorJson = nlohmann::json::parse(editorFile);
		editorFile.close();

		const nlohmann::json editorSettings = editorJson["editor_settings"];
		myMusicIsActive = editorSettings["musicActive"];
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

	void SettingsTool::ToggleVSync(Graphics::GraphicsEngine* aGraphicsEngine)
	{
		bool vsync = aGraphicsEngine->IsVSyncActive();

		if (ImGui::Checkbox("VSync", &vsync))
		{
			aGraphicsEngine->SetVSync(vsync);
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
					Global::SetWindowSizeNextFrame(myMonitorResolution, true);
				}
				else
				{
					Global::SetWindowSizeNextFrame(myWindowSizes[mySelectedWindowSize]);
				}
			}
		}
	}

	void SettingsTool::AdjustRasterizerState()
	{
		ImGui::SetNextItemWidth(200);

		std::array<const char*, static_cast<int>(Graphics::eRasterizerState::Count)> rasterizerStates = {};
		rasterizerStates[static_cast<int>(Graphics::eRasterizerState::BackfaceCulling)] = "BackfaceCulling";
		rasterizerStates[static_cast<int>(Graphics::eRasterizerState::NoFaceCulling)] = "NoFaceCulling";
		rasterizerStates[static_cast<int>(Graphics::eRasterizerState::Wireframe)] = "Wireframe";
		rasterizerStates[static_cast<int>(Graphics::eRasterizerState::WireframeNoCulling)] = "WireframeNoCulling";
		rasterizerStates[static_cast<int>(Graphics::eRasterizerState::FrontFaceCulling)] = "FrontFaceCulling";

		if (ImGui::Combo("RasterizerState", &mySelectedRasterizerState, rasterizerStates.data(), static_cast<int>(rasterizerStates.size())))
		{
			Global::GetGraphicsEngine()->SetRasterizerState(static_cast<Graphics::eRasterizerState>(mySelectedRasterizerState));
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

	void SettingsTool::AdjustEditorStyle()
	{
		std::vector<const char*> editorStyles(3);

		editorStyles[0] = "Simple";
		editorStyles[1] = "Dark";
		editorStyles[2] = "Light";

		static int selectedStyle = 0;

		ImGui::SetNextItemWidth(200);
		if (ImGui::Combo("Editor Style##SettingTool", &selectedStyle, editorStyles.data(), static_cast<int>(editorStyles.size())))
		{
			switch (selectedStyle)
			{
			case 0:
				Simple::ImGuiEngine::SetSimpleStyle();
				break;
			case 1:
				Simple::ImGuiEngine::SetDarkStyle();
				break;
			case 2:
				Simple::ImGuiEngine::SetLightStyle();
				break;
			}
		}
	}

	void SettingsTool::AdjustFPSCap(Graphics::GraphicsEngine* aGraphicsEngine)
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