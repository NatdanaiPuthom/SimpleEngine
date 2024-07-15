#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Windows/SettingsWindow.hpp"
#include "Editor/Editor.hpp"
#include "Engine/ImGui/ImGuiEngine.hpp"
#include "MainSingleton/MainSingleton.hpp"
#include <stdlib.h>

namespace Editor
{
	SettingsWindow::SettingsWindow()
		: mySelectedWindowSize(0)
		, mySelectedRasterizerState(0)
		, myConsoleIsOpen(true)
		, myMusicIsActive(true)
	{
	}

	void SettingsWindow::Init()
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

	void SettingsWindow::Update()
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

	void SettingsWindow::Draw()
	{
		if (ImGui::Begin("Settings##SettingWindow"))
		{
			Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();
			static constexpr unsigned int heightPadding = 2;

			ShowFPS();

			{
				ImGui::SameLine(ImGui::GetWindowWidth() - 100);
				ShowDrawCalls();
			}

			ToggleVSync(graphicsEngine);

			ImGui::SameLine();
			ImGui::Dummy(ImVec2(10, 0));
			ImGui::SameLine();

			ToggleUsingPBR(graphicsEngine);

			AdjustFPSCap(graphicsEngine);
			AdjustRasterizerState();
			AdjustWindowSize();

			ImGui::Dummy(ImVec2(0, heightPadding));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0, heightPadding));

			ToggleConsole();

			if (ImGui::Button("Clear Console##SettingsWindow"))
			{
				system("CLS");
			}

			ImGui::Dummy(ImVec2(0, heightPadding));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0, heightPadding));

			if (ImGui::Checkbox("Render Debug Lines", &EditorEngine::myStaticShouldRenderDebugLines))
			{
			}

			ImGui::Dummy(ImVec2(0, heightPadding));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0, heightPadding));

			ShowMusic();

			ImGui::Dummy(ImVec2(0, heightPadding));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0, heightPadding));

			AdjustEditorStyle();

			ImGui::Dummy(ImVec2(0, heightPadding));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0, heightPadding));

			SetCustomCursorIcon();

			ImGui::Dummy(ImVec2(0, heightPadding));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0, heightPadding));
		}

		ImGui::End();
	}

	void SettingsWindow::UpdateAndFetchCurrentMonitorResolution()
	{
		HMONITOR hMonitor = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY);

		MONITORINFOEX monitorInfo = { sizeof(MONITORINFOEX) };
		GetMonitorInfo(hMonitor, &monitorInfo);

		myMonitorResolution.x = static_cast<unsigned int>(monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left);
		myMonitorResolution.y = static_cast<unsigned int>(monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top);
	}

	void SettingsWindow::LoadDataFromJson()
	{
		const std::string editorJsonFileName = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_EDITOR);
		std::ifstream editorFile(editorJsonFileName);
		assert(editorFile.is_open() && "Failed To Open file");

		const nlohmann::json editorJson = nlohmann::json::parse(editorFile);
		editorFile.close();

		const nlohmann::json editorSettings = editorJson["Editor_Settings"];
		myMusicIsActive = editorSettings["MusicActive"];
	}

	void SettingsWindow::CheckCursorIndexOnce(const std::unordered_map<std::string, const HCURSOR>& aLoadedCursors, int& aSelectedCursor)
	{
		const HCURSOR currentCursor = Global::GetCurrentCustomCursor();

		static bool alreadyRunOnce = false;

		if (alreadyRunOnce == false)
		{
			unsigned int index = 0;

			for (const auto& [name, cursor] : aLoadedCursors)
			{
				if (currentCursor == cursor)
				{
					aSelectedCursor = index;
					break;
				}

				index++;
			}
		}
	}

	void SettingsWindow::ToggleConsole()
	{
		if (ImGui::Checkbox("Show Console##SettingWindow", &myConsoleIsOpen))
		{
			HWND consoleWindow = GetConsoleWindow();

			if (myConsoleIsOpen)
				ShowWindow(consoleWindow, SW_SHOW);
			else
				ShowWindow(consoleWindow, SW_HIDE);
		}
	}

	void SettingsWindow::ToggleUsingPBR(Graphics::GraphicsEngine* aGraphicsEngine)
	{
		bool isUsingPBR = aGraphicsEngine->IsUsingPBR();

		if (ImGui::Checkbox("PBR Render##SettingWindow", &isUsingPBR))
		{
			aGraphicsEngine->SetUsingPBR(isUsingPBR);
		}
	}

	void SettingsWindow::ToggleVSync(Graphics::GraphicsEngine* aGraphicsEngine)
	{
		bool vsync = aGraphicsEngine->IsVSyncActive();

		if (ImGui::Checkbox("VSync##SettingWindow", &vsync))
		{
			aGraphicsEngine->SetVSync(vsync);
		}
	}

	void SettingsWindow::ShowDrawCalls()
	{
		std::string drawCalls = "DrawCalls: " + std::to_string(Global::GetDrawCalls());
		ImGui::Text(drawCalls.c_str());
	}

	void SettingsWindow::ShowFPS()
	{
		std::string fps = "FPS: " + std::to_string(Global::GetFPS());
		ImGui::Text(fps.c_str());
	}

	void SettingsWindow::ShowMusic()
	{
		Simpleton::AudioManager& audioManager = MainSingleton::GetAudioManager();

		const std::string musicName = audioManager.GetMainMusicName();

		if (ImGui::Checkbox("Play Music##SettingWindow", &myMusicIsActive))
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

		char buffer[256];
		memset(buffer, '\0', sizeof(buffer));
		strncpy_s(buffer, musicName.c_str(), sizeof(buffer));
		buffer[sizeof(buffer) - 1] = '\0';

		ImGui::AlignTextToFramePadding();

		ImGui::Text("Music:");
		ImGui::SameLine();
		ImGui::PushItemWidth(200);

		ImGui::BeginDisabled();
		if (ImGui::InputTextWithHint("##MusicSettingWindow", "Drag Drop Music File", buffer, sizeof(buffer)))
		{
		}
		ImGui::EndDisabled();

		if (const ImGuiPayload* currentPayload = ImGui::GetDragDropPayload())
		{
			const std::string payloadData = reinterpret_cast<const char*>(currentPayload->Data);
			const std::string extension = SimpleUtilities::FileManager::GetFileExtension(payloadData);

			if (extension == ".mp3")
			{
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets_Browser"))
					{
						const std::string newMusicName = SimpleUtilities::FileManager::GetFileName(payloadData);
						audioManager.SetMainMusic(newMusicName);
					}
					ImGui::EndDragDropTarget();
				}
			}
		}

		float musicVolume = audioManager.GetMusicVolume();
		if (ImGui::DragFloat("Music Volume##SettingWindow", &musicVolume, 0.01f, 0.0f, 1.0f))
		{
			audioManager.ChangeMusicVolume(musicVolume);
		}
	}

	void SettingsWindow::AdjustWindowSize()
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

		if (ImGui::Combo("WindowSize##SettingWindow", &mySelectedWindowSize, windowSizeAsChar.data(), static_cast<int>(myWindowSizes.size())))
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

	void SettingsWindow::AdjustRasterizerState()
	{
		ImGui::SetNextItemWidth(200);

		std::array<const char*, static_cast<int>(Graphics::eRasterizerState::Count)> rasterizerStates = {};
		rasterizerStates[static_cast<int>(Graphics::eRasterizerState::BackfaceCulling)] = "BackfaceCulling";
		rasterizerStates[static_cast<int>(Graphics::eRasterizerState::NoFaceCulling)] = "NoFaceCulling";
		rasterizerStates[static_cast<int>(Graphics::eRasterizerState::Wireframe)] = "Wireframe";
		rasterizerStates[static_cast<int>(Graphics::eRasterizerState::WireframeNoCulling)] = "WireframeNoCulling";
		rasterizerStates[static_cast<int>(Graphics::eRasterizerState::FrontFaceCulling)] = "FrontFaceCulling";

		if (ImGui::Combo("RasterizerState##SettingWindow", &mySelectedRasterizerState, rasterizerStates.data(), static_cast<int>(rasterizerStates.size())))
		{
			Global::GetGraphicsEngine()->SetRasterizerState(static_cast<Graphics::eRasterizerState>(mySelectedRasterizerState));
		}
	}

	void SettingsWindow::AdjustEditorStyle()
	{
		std::vector<const char*> editorStyles(3);

		editorStyles[0] = "Simple";
		editorStyles[1] = "Dark";
		editorStyles[2] = "Light";

		static int selectedStyle = 0;

		ImGui::SetNextItemWidth(200);
		if (ImGui::Combo("Editor Style##SettingWindow", &selectedStyle, editorStyles.data(), static_cast<int>(editorStyles.size())))
		{
			switch (selectedStyle)
			{
			case 0:
				Simple::ImGuiEngine::SetEditorStyle(Simple::eImGuiEditorStyle::Simple);
				break;
			case 1:
				Simple::ImGuiEngine::SetEditorStyle(Simple::eImGuiEditorStyle::Dark);
				break;
			case 2:
				Simple::ImGuiEngine::SetEditorStyle(Simple::eImGuiEditorStyle::Light);
				break;
			default:
				Simple::ImGuiEngine::SetEditorStyle(Simple::eImGuiEditorStyle::Simple);
				break;
			}
		}
	}

	void SettingsWindow::AdjustFPSCap(Graphics::GraphicsEngine* aGraphicsEngine)
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

			if (ImGui::Combo("FPS Cap##SettingWindow", &selectedFPSLevelCap, fpsCapAsConstChar.data(), static_cast<int>(fpsCapAsConstChar.size())))
			{
				if (selectedFPSLevelCap == 1)
					aGraphicsEngine->SetVSync(true);
				else
					aGraphicsEngine->SetFPSLevelCap(selectedFPSLevelCap);
			}
		}
	}

	void SettingsWindow::SetCustomCursorIcon()
	{
		const std::unordered_map<std::string, const HCURSOR>& loadedCursors = Global::GetLoadedCustomCursors();
		std::vector<std::string> cursorNames;
		std::string cursors;

		for (const auto& [name, cursor] : loadedCursors)
		{
			cursorNames.push_back(name);
			cursors += name;
			cursors += '\0';
		}

		cursors += '\0';

		static int selectedCursor = 0;

		CheckCursorIndexOnce(loadedCursors, selectedCursor);

		ImGui::SetNextItemWidth(200);

		if (ImGui::Combo("Cursors##SettingWindow", &selectedCursor, cursors.c_str()))
		{
			Global::SetCustomCursor(cursorNames[selectedCursor]);
		}
	}
}