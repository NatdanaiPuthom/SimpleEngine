#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Menu/MainMenuBar.hpp"
#include "Editor/Tools/SettingsTool.hpp"
#include "Editor/Tools/CameraTool.hpp"

Tool::MainMenuBar::MainMenuBar()
	: mySettingToolActive(false)
	, myCameraToolActive(false)
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
	if (SimpleUtilities::InputManager::GetInstance().IsKeyPressed(VK_F1))
	{
		mySettingToolActive = !mySettingToolActive;
	}

	if (mySettingToolActive)
	{
		mySettingsTool->Draw();
	}

	if (myCameraToolActive)
	{
		myCameraTool->Draw();
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
	myCameraToolActive = settings["camera_tool"]["active"];
}