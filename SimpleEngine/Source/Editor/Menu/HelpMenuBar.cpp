#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Menu/HelpMenuBar.hpp"
#include "Editor/HelpWindows/CameraHelpWindow.hpp"

namespace Editor
{
	HelpMenuBar::HelpMenuBar()
	{
	}

	HelpMenuBar::~HelpMenuBar()
	{
	}

	void HelpMenuBar::Init()
	{
		myCameraHelpWindow = std::make_unique<CameraHelpWindow>();

		LoadSettingsFromJson();

		myCameraHelpWindow->Init();
	}

	void HelpMenuBar::Update()
	{
		myCameraHelpWindow->Update();
	}

	void HelpMenuBar::Draw()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Help"))
			{
				ImGui::MenuItem("Camera Controls", "", &myCameraHelpWindow->myIsActive);
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		if (myCameraHelpWindow->myIsActive == true)
		{
			myCameraHelpWindow->Draw();
		}
	}

	void HelpMenuBar::LoadSettingsFromJson(const std::string& aFileName)
	{
		std::ifstream file(aFileName);
		assert(file.is_open() && "Failed to open file");

		const nlohmann::json json = nlohmann::json::parse(file);
		file.close();

		const nlohmann::json settings = json["Editor_Settings"]["Help_Windows"];

		myCameraHelpWindow->myIsActive = settings["Show_Camera"];
	}
}