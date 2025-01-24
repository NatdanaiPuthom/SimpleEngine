#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Menu/MainMenuBar.hpp"
#include "Editor/FlyScript/NodeScriptingWindow.hpp"
#include "MainSingleton/MainSingleton.hpp"
#include "External/dearimgui/imguizmo/ImGuizmo.h"
#include "Editor/Editor.hpp"

namespace Editor
{
	MainMenuBar::MainMenuBar()
	{
	}

	MainMenuBar::~MainMenuBar()
	{
	}

	bool ActiveWindowData::IsWindowActive(eWindowType aWindowType) const
	{
		return myIsWindowActive[static_cast<size_t>(aWindowType)];
	}

	void ActiveWindowData::SetActiveWindow(eWindowType aWindowType, const bool aIsActive)
	{
		assert(aWindowType != eWindowType::Count);

		for (bool& isActive : myIsWindowActive)
		{
			isActive = false;
		}

		myIsWindowActive[static_cast<size_t>(aWindowType)] = aIsActive;
	}

	void MainMenuBar::Init()
	{
		myNodeScriptingWindow = std::make_unique<NodeScriptingWindow>();

		LoadSettingsFromJson();

		myNodeScriptingWindow->Init();
	}

	void MainMenuBar::Update()
	{
		Simpleton::InputManager& inputManager = MainSingleton::GetInputManager();

		static const char* const windowNames[] = { "Editor", "NodeScript" };
		static const char* const keyShortCuts[] = { "F1", "F4" };

		for (unsigned int i = 0; i < static_cast<unsigned int>(eWindowType::Count); ++i)
		{
			if (inputManager.IsKeyPressed(VK_F1 + i))
			{
				myActiveWindowData.SetActiveWindow(static_cast<eWindowType>(i), !myActiveWindowData.IsWindowActive(static_cast<eWindowType>(i)));
			}
		}

		if (MainSingleton::GetInputManager().IsKeyPressed(VK_F5))
		{
			if (Global::IsFullScreen())
			{
				Global::SetWindowSizeNextFrame({ 1280,720 }, false);
			}
			else
			{
				Global::SetWindowSizeNextFrame({ 0,0 }, true);
			}
		}

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Windows"))
			{
				for (unsigned int i = 0; i < static_cast<unsigned int>(eWindowType::Count); ++i)
				{
					if (ImGui::MenuItem(windowNames[i], keyShortCuts[i], myActiveWindowData.IsWindowActive(static_cast<eWindowType>(i))))
					{
						myActiveWindowData.SetActiveWindow(static_cast<eWindowType>(i), !myActiveWindowData.IsWindowActive(static_cast<eWindowType>(i)));
					}
				}

				MenuItemFullScreen();

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

	void MainMenuBar::Draw()
	{
		
			

		DrawTools();
	}

	void MainMenuBar::MenuItemFullScreen()
	{
		bool isFullScreen = Global::IsFullScreen();

		if (ImGui::MenuItem("FullScreen", "F5", &isFullScreen))
		{
			if (Global::IsFullScreen())
			{
				Global::SetWindowSizeNextFrame({ 1280,720 }, false);
			}
			else
			{
				Global::SetWindowSizeNextFrame({ 0,0 }, true);
			}
		}
	}

	void MainMenuBar::DrawTools()
	{
		if (myActiveWindowData.IsWindowActive(eWindowType::NodeScript))
		{
			myNodeScriptingWindow->Draw();
		}
	}

	void MainMenuBar::LoadSettingsFromJson()
	{
		const std::string filename = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_EDITOR);

		std::ifstream file(filename);
		assert(file.is_open() && "Failed To Open File");

		const nlohmann::json json = nlohmann::json::parse(file);
		file.close();

		const nlohmann::json settings = json["Editor_Settings"];

		myActiveWindowData.SetActiveWindow(eWindowType::Editor, settings["Editor"]["Active"]);
	}
}