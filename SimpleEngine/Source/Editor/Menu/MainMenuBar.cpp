#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Menu/MainMenuBar.hpp"
#include "Editor/Tools/SettingsTool.hpp"
#include "Editor/Windows/HierarchyWindow.hpp"
#include "Editor/Windows/AssetWindow.hpp"
#include "Editor/Windows/DeferredSceneWindow.hpp"
#include "Editor/Windows/PostProcessingWindow.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace Editor
{
	bool MainMenuBar::myStaticNodeScriptWindowActive = false;

	MainMenuBar::MainMenuBar()
		: myEditorWindowActive(false)
		, myDeferredWindowActive(false)
		, myPostProcessWindowActive(false)
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
		myPostProcessWindow = std::make_unique<PostProcessingWindow>();

		LoadSettingsFromJson();

		mySettingsTool->Init();
		myAssetWindow->Init();
		myHierarchyWindow->Init();
		myDeferredSceneWindow->Init();
		myPostProcessWindow->Init();
	}

	void MainMenuBar::Update()
	{
		Simpleton::InputManager& inputManager = MainSingleton::GetInputManager();

		bool* const windowActive[] = { &myEditorWindowActive, &myDeferredWindowActive, &myPostProcessWindowActive, &myStaticNodeScriptWindowActive };
		static const char* const windowNames[] = { "Editor", "Deferred", "PostProcess", "NodeScript" };
		static const char* const keyShortCuts[] = { "F1", "F2", "F3", "F4" };

		for (int i = 0; i < sizeof(windowActive) / sizeof(windowActive[0]); ++i)
		{
			if (inputManager.IsKeyPressed(VK_F1 + i))
			{
				*windowActive[i] = !(*windowActive[i]);

				for (int j = 0; j < sizeof(windowActive) / sizeof(windowActive[0]); ++j)
				{
					if (j != i)
					{
						*windowActive[j] = false;
					}
				}
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
				for (unsigned int i = 0; i < sizeof(windowActive) / sizeof(windowActive[0]); ++i)
				{
					if (ImGui::MenuItem(windowNames[i], keyShortCuts[i], *&windowActive[i]))
					{
						for (unsigned int j = 0; j < sizeof(windowActive) / sizeof(windowActive[0]); ++j)
						{
							if (j != i)
							{
								*windowActive[j] = false;
							}
						}
					}
				}

				MenuItemFullScreen();

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		if (myEditorWindowActive == true)
		{
			mySettingsTool->Update();
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
				ImTextureID textureID = Global::GetGraphicsEngine()->GetShaderResourceView(Graphics::eRenderTargetType::PostProcessing).Get();
				ImVec2 size = ImGui::GetContentRegionAvail();
				ImGui::Image(textureID, size);
			}
			ImGui::End();
		}
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

		if (myPostProcessWindowActive == true)
		{
			myPostProcessWindow->Draw();
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