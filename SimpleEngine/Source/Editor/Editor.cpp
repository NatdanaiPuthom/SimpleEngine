#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Editor.hpp"

#include "MainSingleton/MainSingleton.hpp"

#include "Editor/Core/MainMenuItem.hpp"
#include "Editor/Core/MainMenuItemButton.hpp"
#include "Editor/Core/MainMenuItemList.hpp"
#include "Editor/Core/MainMenuItemSelectable.hpp"

#include "Editor/PopUps/Help/CameraControlsGuidePopUp.hpp"
#include "Editor/PopUps/Settings/CameraSettingsPopUp.hpp"
#include "Editor/PopUps/Settings/AudioSettingsPopUp.hpp"
#include "Editor/PopUps/Settings/GraphicsSettingsPopUp.hpp"

#include "Editor/MenuItems/Scene/SceneItemLoadSelectable.hpp"
#include "Editor/MenuItems/Scene/SceneItemSaveButton.hpp"
#include "Editor/MenuItems/Scene/SceneItemCreateNewButton.hpp"
#include "Editor/MenuItems/Scene/SceneItemCreateCopyButton.hpp"
#include "Editor/MenuItems/Scene/SceneItemReloadButton.hpp"
#include "Editor/MenuItems/Scene/SceneItemSetAsStartButton.hpp"

#include "Editor/PopUps/PostProcessPopUp.hpp"

#include "Engine/ImGui/ImGuiEngine.hpp" //TempPlayMenuBar

namespace Editor
{
	size_t EditorEngine::mySelectedEntityID = static_cast<size_t>(-1);

	EditorEngine::EditorEngine()
	{
	}

	EditorEngine::~EditorEngine()
	{
	}

	void EditorEngine::Init()
	{
		SetUpSceneTab();
		SetupSettingsTab();
		SetUpHelpTab();

		auto a = AddPopUpWindow<PostProcessPopUp>();
		a->SetActive(true);

		for (const std::shared_ptr<PopUp> popUp : myPopUpWindows)
		{
			popUp->Init();
		}
	}

	void EditorEngine::Update()
	{
		if (MainSingleton::GetSceneManager().GetIsPlaying() == false)
		{
			Global::GetGraphicsEngine()->GetEditorCamera()->Update(Global::GetDeltaTime(), Global::GetEngineHWND());
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

		for (const std::shared_ptr<PopUp> popUp : myPopUpWindows)
		{
			popUp->Update();
		}

		for (const std::shared_ptr<MainMenuItemTab> tab : myMainMenuTabs)
		{
			tab->InternalUpdate();
		}
	}

	void EditorEngine::Render()
	{
		for (const std::shared_ptr<MainMenuItemTab> tab : myMainMenuTabs)
		{
			tab->Render();
		}

		for (const std::shared_ptr<PopUp> popUp : myPopUpWindows)
		{
			if (popUp->IsActive())
			{
				popUp->Render();
			}
		}

		TempPlayMenuBar();


		//{	//Render Orientation Cube  
		//	//TO-DO(v11.4.4): Make own class for this
  //          const Graphics::Camera* camera = Global::GetGraphicsEngine()->GetCurrentCamera();  
  //          Math::Matrix4x4f view = camera->GetViewMatrix();  
  //          ImVec2 windowPos = ImGui::GetWindowPos();  
  //          ImGuizmo::ViewManipulate(&view(1, 1), 16, ImVec2(windowPos.x + 775, windowPos.y + 375), ImVec2(64, 64), 0x00000000);
		//}
	}

	void EditorEngine::SetUpSceneTab()
	{
		std::shared_ptr<MainMenuItemTab> sceneTab = AddMenuTab<MainMenuItemTab>();

		std::shared_ptr<SceneItemSaveButton> saveButton = std::make_shared<SceneItemSaveButton>("Save");
		std::shared_ptr<SceneItemLoadSelectable> loadSelector = std::make_shared<SceneItemLoadSelectable>("Load");
		std::shared_ptr<MainMenuItemList> createButtonList = std::make_shared<MainMenuItemList>("Create");
		std::shared_ptr<SceneItemReloadButton> reloadButton = std::make_shared<SceneItemReloadButton>("Reload");
		std::shared_ptr<SceneItemSetAsStartButton> setAsActiveButton = std::make_shared<SceneItemSetAsStartButton>("Set As Start");

		const std::vector<std::string> sceneNames = SimpleUtilities::FileManager::GetFileNamesFromDirectory(SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_SCENES));

		for (const auto& name : sceneNames)
		{
			loadSelector->AddString(name);
		}

		std::shared_ptr<SceneItemCreateNewButton> testCreateNewButton = std::make_shared<SceneItemCreateNewButton>("New");
		std::shared_ptr<SceneItemCreateNewCopyButton> testCreateCopyButton = std::make_shared<SceneItemCreateNewCopyButton>("Copy");

		sceneTab->AddChild(saveButton);
		sceneTab->AddChild(loadSelector);
		sceneTab->AddChild(createButtonList);
		sceneTab->AddChild(reloadButton);
		sceneTab->AddChild(setAsActiveButton);

		createButtonList->AddChild(testCreateNewButton);
		createButtonList->AddChild(testCreateCopyButton);

		sceneTab->SetWindowName("Scene");
	}

	void EditorEngine::SetupSettingsTab()
	{
		std::shared_ptr<MainMenuItemTab> settingsTab = AddMenuTab<MainMenuItemTab>();

		std::shared_ptr<MainMenuItemPopUp> audioSettingButton = std::make_shared<MainMenuItemPopUp>("Audio");
		std::shared_ptr<MainMenuItemPopUp> cameraSettingButton = std::make_shared<MainMenuItemPopUp>("Camera");
		std::shared_ptr<MainMenuItemPopUp> graphicsSettingButton = std::make_shared<MainMenuItemPopUp>("Graphics");

		std::shared_ptr<CameraSettingsPopUp> cameraSettingsPopUp = AddPopUpWindow<CameraSettingsPopUp>();
		std::shared_ptr<AudioSettingsPopUp> audioSettingsPopUp = AddPopUpWindow<AudioSettingsPopUp>();
		std::shared_ptr<GraphicsSettingsPopUp> graphicsSettingsPopUp = AddPopUpWindow<GraphicsSettingsPopUp>();

		settingsTab->AddChild(audioSettingButton);
		settingsTab->AddChild(cameraSettingButton);
		settingsTab->AddChild(graphicsSettingButton);

		audioSettingButton->AddPopUpWindows(audioSettingsPopUp);
		cameraSettingButton->AddPopUpWindows(cameraSettingsPopUp);
		graphicsSettingButton->AddPopUpWindows(graphicsSettingsPopUp);

		settingsTab->SetWindowName("Settings");
		cameraSettingsPopUp->SetWindowName("Camera Settings");
		audioSettingsPopUp->SetWindowName("Audio Settings");
		graphicsSettingsPopUp->SetWindowName("Graphics Settings");
	}

	void EditorEngine::SetUpHelpTab()
	{
		std::shared_ptr<MainMenuItemTab> helpTab = AddMenuTab<MainMenuItemTab>();

		std::shared_ptr<MainMenuItemPopUp> cameraHelpButton = std::make_shared<MainMenuItemPopUp>("Camera Controls");
		std::shared_ptr<CameraControlsGuidePopUp> cameraControlsHelpPopUp = AddPopUpWindow<CameraControlsGuidePopUp>();

		helpTab->AddChild(cameraHelpButton);
		cameraHelpButton->AddPopUpWindows(cameraControlsHelpPopUp);

		helpTab->SetWindowName("Help");
		cameraControlsHelpPopUp->SetWindowName("Editor Camera");
	}

	void EditorEngine::TempPlayMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			const float distanceFromStart = ImGui::GetWindowWidth() - ImGui::GetContentRegionAvail().x;

			ImGui::Dummy(ImVec2(-distanceFromStart + ImGui::GetWindowWidth() * 0.5f - 38.0f, 0));

			Simpleton::SceneManager& sceneManager = MainSingleton::GetSceneManager();
			Graphics::GraphicsEngine* graphicsEngine = Global::GetGraphicsEngine();

			const bool isPlaying = sceneManager.GetIsPlaying();

			if (isPlaying == true)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImColor(1.0f, 0.0f, 0.0f, 1.0f).Value);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(0.6f, 0.0f, 0.0f, 1.0f).Value);
				Simple::ImGuiEngine::SetEditorMode(Simple::eImGuiEditorMode::Playing);

				ECS::EntityComponentSystem& ecs = sceneManager.GetCurrentECS();
				const std::unordered_set<ECS::EntityID>& cameraEntityIDs = ecs.GetEntityIDsWithThisComponent<ECS::CameraComponent>();

				if (cameraEntityIDs.empty() == false)
				{
					ECS::CameraComponent* cameraComponent = ecs.GetEntity(*cameraEntityIDs.begin()).GetComponent<ECS::CameraComponent>();

					if (cameraComponent != nullptr)
					{
						graphicsEngine->SetCamera(&cameraComponent->camera);
					}
				}
			}
			else
			{
				Simple::ImGuiEngine::SetEditorMode(Simple::eImGuiEditorMode::Default);
				graphicsEngine->SetCamera(graphicsEngine->GetEditorCamera().get());
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10);
			if (ImGui::Button(ICON_FA_PLAY))
			{
				sceneManager.SetIsPlaying(!isPlaying);
			}
			ImGui::PopStyleVar();

			if (isPlaying == true)
			{
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
			}

			ImGui::EndMainMenuBar();
		}
	}
}
