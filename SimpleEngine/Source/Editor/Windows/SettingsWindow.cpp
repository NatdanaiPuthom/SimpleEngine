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
		, myConsoleIsOpen(true)
	{
	}

	void SettingsWindow::Init()
	{
		UpdateAndFetchCurrentMonitorResolution();

		myWindowSizes.push_back(Math::Vector2ui(1280, 720));
		myWindowSizes.push_back(Math::Vector2ui(1600, 900));
		myWindowSizes.push_back(Math::Vector2ui(1920, 1080));

		if (myWindowSizes.back().x < myMonitorResolution.x &&
			myWindowSizes.back().y < myMonitorResolution.y)
		{
			myWindowSizes.push_back(myMonitorResolution);
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
		if (ImGui::Begin("General Settings##SettingWindow"))
		{
			static constexpr unsigned int heightPadding = 2;

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

	void SettingsWindow::ToggleVSync(Graphics::GraphicsEngine* aGraphicsEngine)
	{
		Graphics::GenericDataManager* graphicsGenericDataManager = aGraphicsEngine->GetGenericDataManager();
		bool vsync = graphicsGenericDataManager->IsVSyncOn();

		if (ImGui::Checkbox("VSync##SettingWindow", &vsync))
		{
			graphicsGenericDataManager->SetVSync(vsync);
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