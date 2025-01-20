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
}