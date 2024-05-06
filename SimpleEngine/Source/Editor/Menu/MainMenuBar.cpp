#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Menu/MainMenuBar.hpp"
#include "Editor/Tools/SettingsTool.hpp"
#include "Editor/Windows/HierarchyWindow.hpp"
#include "Editor/Windows/AssetWindow.hpp"

namespace Editor
{
	MainMenuBar::MainMenuBar()
		: myEditorWindowActive(false)
	{
	}

	MainMenuBar::~MainMenuBar()
	{
	}

	void MainMenuBar::Init()
	{
		mySettingsTool = std::make_unique<SettingsTool>();
		myAssetWindow = std::make_unique<AssetWindow>();
		myHierarchyWindow = std::make_unique<HierarchyWindow>();

		LoadSettingsFromJson();

		mySettingsTool->Init();
		myAssetWindow->Init();
		myHierarchyWindow->Init();
	}

	void MainMenuBar::Update()
	{
		myHierarchyWindow->Update();
	}

	void MainMenuBar::Draw()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Editor"))
			{
				ImGui::MenuItem("Show", "F1", &myEditorWindowActive);
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		DrawTools();

		if (myEditorWindowActive == true)
		{
			if (ImGui::Begin("Scene", 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImTextureID textureID = Global::GetGraphicsEngine()->GetImGuiShaderResourceView().Get();
				ImVec2 size = ImGui::GetContentRegionAvail();
				ImGui::Image(textureID, size);
			}
			ImGui::End();
		}
	}

	void MainMenuBar::DrawTools()
	{
		if (MainSingleton::GetInputManager().IsKeyPressed(VK_F1))
		{
			myEditorWindowActive = !myEditorWindowActive;
		}

		if (myEditorWindowActive)
		{
			myAssetWindow->Draw();
			mySettingsTool->Draw();
			myHierarchyWindow->Draw(); //TO-DO(v9.31.1): For now HierachyWindow should always be run last due to removing Entities during run time. Fix/Look into it in future
		}
	}

	void MainMenuBar::LoadSettingsFromJson()
	{
		const std::string filename = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_EDITOR);

		std::ifstream file(filename);
		assert(file.is_open() && "Failed To Open File");

		const nlohmann::json json = nlohmann::json::parse(file);
		file.close();

		const nlohmann::json settings = json["editor_settings"];

		myEditorWindowActive = settings["editor"]["active"];
		myHierarchyWindow->myIsActive = settings["windows"]["show_inspector"];
	}
}