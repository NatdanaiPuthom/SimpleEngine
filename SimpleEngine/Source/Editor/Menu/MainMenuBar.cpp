#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Menu/MainMenuBar.hpp"
#include "Editor/Tools/SettingsTool.hpp"
#include "Editor/Windows/HierarchyWindow.hpp"
#include "Editor/Windows/AssetWindow.hpp"
#include "Editor/Windows/DeferredSceneWindow.hpp"

namespace Editor
{
	bool MainMenuBar::staticNodeScriptWindowActive = false;

	MainMenuBar::MainMenuBar()
		: myEditorWindowActive(false)
		, myDeferredWindowActive(false)
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
		myDeferredSceneWindow = std::make_unique<DeferredSceneWindow>();

		LoadSettingsFromJson();

		mySettingsTool->Init();
		myAssetWindow->Init();
		myHierarchyWindow->Init();
		myDeferredSceneWindow->Init();
	}

	void MainMenuBar::Update()
	{
		Simpleton::InputManager& inputManager = MainSingleton::GetInputManager();

		if (inputManager.IsKeyPressed(VK_F1))
		{
			myEditorWindowActive = !myEditorWindowActive;
		}

		if (inputManager.IsKeyPressed(VK_F2))
		{
			myDeferredWindowActive = !myDeferredWindowActive;
		}

		if (inputManager.IsKeyPressed(VK_F3))
		{
			staticNodeScriptWindowActive = !staticNodeScriptWindowActive;
		}

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Windows"))
			{
				ImGui::MenuItem("Editor", "F1", &myEditorWindowActive);
				ImGui::MenuItem("Deferred", "F2", &myDeferredWindowActive);
				ImGui::MenuItem("NodeScript", "F3", &staticNodeScriptWindowActive);
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		if (myEditorWindowActive == true)
		{
			myHierarchyWindow->Update();
			myAssetWindow->Update();
		}
	}

	void MainMenuBar::Draw()
	{
		DrawTools();

		if (myEditorWindowActive == true)
		{
			if (ImGui::Begin("Scene", 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImTextureID textureID = Global::GetGraphicsEngine()->GetShaderResourceView(Graphics::eRenderTargetType::Deferred).Get();
				ImVec2 size = ImGui::GetContentRegionAvail();
				ImGui::Image(textureID, size);
			}
			ImGui::End();
		}
	}

	void MainMenuBar::DrawTools()
	{
		if (myEditorWindowActive)
		{
			myAssetWindow->Draw();
			mySettingsTool->Draw();
			myHierarchyWindow->Draw(); //TO-DO(v9.31.1): For now HierachyWindow should always be run last due to removing Entities during run time. Fix/Look into it in future
		}

		if (myDeferredWindowActive == true)
		{
			myDeferredSceneWindow->Draw();
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