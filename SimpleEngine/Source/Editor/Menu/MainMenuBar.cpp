#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Menu/MainMenuBar.hpp"
#include "Editor/Tools/SettingsTool.hpp"
#include "Editor/Tools/CameraTool.hpp"
#include "Editor/Tools/InspectorWindow.hpp"
#include "Editor/Tools/HierarchyWindow.hpp"
#include "Editor/Tools/AssetWindow.hpp"

Tool::MainMenuBar::MainMenuBar()
	: mySettingToolActive(false)
	, myDebugMenuActive(false)
{
}

Tool::MainMenuBar::~MainMenuBar()
{
}

void Tool::MainMenuBar::Init()
{
	LoadSettingsFromJson();

	mySettingsTool = std::make_unique<Tool::SettingsTool>();
	myCameraTool = std::make_unique<Tool::CameraTool>();
	myInspectorWindow = std::make_unique<Editor::InspectorWindow>();
	myHierarchyWindow = std::make_unique<Editor::HierarchyWindow>();
	myAssetWindow = std::make_unique<Editor::AssetWindow>();

	mySettingsTool->Init();
	myCameraTool->Init();
	myInspectorWindow->Init();
	myHierarchyWindow->Init();
	myAssetWindow->Init();
}

void Tool::MainMenuBar::Draw()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Debug"))
		{
			ImGui::MenuItem("Settings", "", &mySettingToolActive);	
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	DrawTools();
}

void Tool::MainMenuBar::DrawTools()
{
	if (SimpleUtilities::InputManager::GetInstance().IsKeyPressed(VK_F1))
	{
		mySettingToolActive = !mySettingToolActive;
	}

	if (mySettingToolActive)
	{
		myHierarchyWindow->Draw();
		myInspectorWindow->Draw();
		myAssetWindow->Draw();
		myCameraTool->Draw();
		mySettingsTool->Draw();
	}
}

void Tool::MainMenuBar::LoadSettingsFromJson()
{
	const std::string filename = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_EDITOR);

	std::ifstream file(filename);
	assert(file.is_open() && "Failed To Open File");

	const nlohmann::json json = nlohmann::json::parse(file);
	file.close();

	const nlohmann::json settings = json["editor_settings"];

	mySettingToolActive = settings["setting_tool"]["active"];
}