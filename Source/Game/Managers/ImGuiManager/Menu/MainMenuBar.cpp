#include "Game/Precomplier/stdafx.h"
#include "Game/Managers/ImGuiManager/Menu/MainMenuBar.hpp"

Tool::MainMenuBar::MainMenuBar()
{
}

void Tool::MainMenuBar::Draw()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("New");
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Settings"))
		{
			ImGui::MenuItem("Test");
			ImGui::EndMenu();
		}
	}

	ImGui::EndMainMenuBar();
}