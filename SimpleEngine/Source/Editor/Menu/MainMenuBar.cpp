#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Menu/MainMenuBar.hpp"
#include "Editor/Windows/SettingsWindow.hpp"
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
		mySettingsWindow = std::make_unique<SettingsWindow>();
		myAssetWindow = std::make_unique<AssetWindow>();
		myHierarchyWindow = std::make_unique<HierarchyWindow>();
		myDeferredSceneWindow = std::make_unique<DeferredSceneWindow>();
		myPostProcessWindow = std::make_unique<PostProcessingWindow>();

		LoadSettingsFromJson();

		mySettingsWindow->Init();
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
			mySettingsWindow->Update();
			myHierarchyWindow->Update();
			myAssetWindow->Update();
		}
	}

	void MainMenuBar::Draw()
	{
		if (myEditorWindowActive == true)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 3));
			if (ImGui::Begin("Scene##MainMenuBar", 0, ImGuiWindowFlags_NoScrollbar))
			{
				Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();
				const Graphics::eRasterizerState currentRasterizerState = graphicsEngine->GetCurrentRasterizerState();

				ImTextureID textureID = graphicsEngine->GetShaderResourceView(Graphics::eRenderTargetType::PostProcessing).Get();

				if (currentRasterizerState != Graphics::eRasterizerState::BackfaceCulling
					&& currentRasterizerState != Graphics::eRasterizerState::NoFaceCulling)
				{
					textureID = graphicsEngine->GetShaderResourceView(Graphics::eRenderTargetType::Deferred).Get();
				}

				ImVec2 size = ImGui::GetContentRegionAvail();
				ImGui::Image(textureID, size);
			}
			ImGui::End();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
		}

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
		if (myEditorWindowActive)
		{
			mySettingsWindow->Draw();
			myAssetWindow->Draw();
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

		const nlohmann::json settings = json["Editor_Settings"];

		myEditorWindowActive = settings["Editor"]["Active"];
		myHierarchyWindow->myIsActive = settings["Windows"]["Show_Inspector"];
	}
}