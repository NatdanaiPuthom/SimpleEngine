#include "Game/Precomplied/GamePch.hpp"
#include "Game/Managers/ImGuiManager/Menu/MainMenuBar.hpp"
#include "Game/Managers/ImGuiManager/Tools/SettingsTool.hpp"
#include "Game/Managers/ImGuiManager/Tools/CameraTool.hpp"

Tool::MainMenuBar::MainMenuBar()
	: mySettingToolActive(false)
	, myCameraToolActive(false)
{
}

Tool::MainMenuBar::~MainMenuBar()
{
}

void Tool::MainMenuBar::Init()
{
	mySettingsTool = std::make_unique<Tool::SettingsTool>();
	myCameraTool = std::make_unique<Tool::CameraTool>();

	mySettingsTool->Init();
	myCameraTool->Init();
}

void Tool::MainMenuBar::Draw()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Debug"))
		{
			ImGui::MenuItem("Settings", "", &mySettingToolActive);
			ImGui::MenuItem("Camera", "", &myCameraToolActive);			
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	DrawTools();
}

void Tool::MainMenuBar::DrawTools()
{
	if (mySettingToolActive)
	{
		mySettingsTool->Draw();
	}

	if (myCameraToolActive)
	{
		myCameraTool->Draw();
	}
}