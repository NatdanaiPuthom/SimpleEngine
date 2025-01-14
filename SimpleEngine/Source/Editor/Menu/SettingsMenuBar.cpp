#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Menu/SettingsMenuBar.hpp"
#include "Editor/Windows/SettingsWindow.hpp"

namespace Editor
{
	SettingsMenuBar::SettingsMenuBar()
	{
	}

	SettingsMenuBar::~SettingsMenuBar()
	{
	}

	void SettingsMenuBar::Init()
	{
		mySettingWindow = std::make_unique<SettingsWindow>();
		mySettingWindow->Init();
		mySettingWindow->myIsActive = false;
	}

	void SettingsMenuBar::Update()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Settings##SettingsWindow"))
			{
				ImGui::MenuItem("General", "", &mySettingWindow->myIsActive);
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

	void SettingsMenuBar::Draw()
	{
		if (mySettingWindow->myIsActive == true)
		{
			mySettingWindow->Draw();
		}
	}

	void SettingsMenuBar::LoadSettingsFromJson(const std::string& /*aFileName*/)
	{
	}
}
