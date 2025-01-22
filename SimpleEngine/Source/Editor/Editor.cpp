#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Editor.hpp"

#include "MainSingleton/MainSingleton.hpp"

#include "Editor/PopUps/Help/CameraControlsGuidePopUp.hpp"
#include "Editor/PopUps/Settings/CameraSettingsPopUp.hpp"
#include "Editor/PopUps/Settings/AudioSettingsPopUp.hpp"
#include "Editor/PopUps/Settings/GraphicsSettingsPopUp.hpp"

#include "Editor/MainMenuTabs/SceneTab.hpp"
#include "Editor/MainMenuTabs/Settings.hpp"

#include "Editor/MainMenuItems/Settings/AudioItem.hpp"
#include "Editor/MainMenuItems/Settings/CameraSettingsItem.hpp"
#include "Editor/MainMenuItems/Settings/GraphicsSettingsItem.hpp"
#include "Editor/MainMenuItems/Scene/SceneItemSave.hpp"
#include "Editor/MainMenuItems/Scene/SceneItemLoad.hpp"
#include "Editor/MainMenuItems/Scene/SceneItemCreate.hpp"
#include "Editor/MainMenuItems/Scene/SceneItemReload.hpp"
#include "Editor/MainMenuItems/Scene/SceneItemSetStart.hpp"

#include "Editor/Core/TestTestTest.hpp"

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
		SetUpHelpTab();
		/*SetUpSceneTab();
		SetupSettingsTab();
		*/

		for (const std::shared_ptr<PopUp> popUp : myPopUpWindows)
		{
			popUp->Init();
		}

	}

	void EditorEngine::Update()
	{
		/*if (MainSingleton::GetSceneManager().GetIsPlaying() == false)
		{
			Global::GetGraphicsEngine()->GetEditorCamera()->Update(Global::GetDeltaTime(), Global::GetEngineHWND());
		}*/

		/*	if (MainSingleton::GetInputManager().IsKeyPressed(VK_F5))
			{
				if (Global::IsFullScreen())
				{
					Global::SetWindowSizeNextFrame({ 1280,720 }, false);
				}
				else
				{
					Global::SetWindowSizeNextFrame({ 0,0 }, true);
				}
			}*/

		for (const std::shared_ptr<PopUp> popUp : myPopUpWindows)
		{
			popUp->Update();
		}

		for (const std::shared_ptr<MainMenuItemParent> tab : myMainMenuTabParents)
		{
			tab->InternalUpdate();
		}
	}

	void EditorEngine::Render()
	{
		for (const std::shared_ptr<MainMenuItemParent> tab : myMainMenuTabParents)
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
		std::shared_ptr<SceneTab> sceneTab = AddMenuTab<SceneTab>();
		sceneTab->SetWindowName("Scene");

		std::shared_ptr<SceneItemSave> sceneSaveItem = sceneTab->AddChildren<SceneItemSave>();
		std::shared_ptr<SceneItemLoad> sceneLoadItem = sceneTab->AddChildren<SceneItemLoad>();
		std::shared_ptr<SceneItemCreate> sceneCreateItem = sceneTab->AddChildren<SceneItemCreate>();
		std::shared_ptr<SceneItemReload> sceneReloadItem = sceneTab->AddChildren<SceneItemReload>();
		std::shared_ptr<SceneItemSetAsStart> sceneSetAsStartItem = sceneTab->AddChildren<SceneItemSetAsStart>();

		sceneSaveItem->SetWindowName("Save");
		sceneLoadItem->SetWindowName("Load");
		sceneCreateItem->SetWindowName("Create");
		sceneReloadItem->SetWindowName("Reload");
		sceneSetAsStartItem->SetWindowName("Set As Start");

		sceneSaveItem->SetHotKeyShortCutText("Ctrl + S");

		sceneReloadItem->SetShowToolTips(true);
		sceneSetAsStartItem->SetShowToolTips(true);
	}

	void EditorEngine::SetupSettingsTab()
	{
		std::shared_ptr<Settings> settingsTab = AddMenuTab<Settings>();

		std::shared_ptr<AudioItem> settingsAudioItem = settingsTab->AddChildren<AudioItem>();
		std::shared_ptr<GraphicsSettingsItem> settingsGraphicsItem = settingsTab->AddChildren<GraphicsSettingsItem>();

		std::shared_ptr<CameraSettingsPopUp> cameraSettingsPopUp = AddPopUpWindow<CameraSettingsPopUp>();
		std::shared_ptr<AudioSettingsPopUp> audioSettingsPopUp = AddPopUpWindow<AudioSettingsPopUp>();
		std::shared_ptr<GraphicsSettingsPopUp> graphicsSettingsPopUp = AddPopUpWindow<GraphicsSettingsPopUp>();

		settingsAudioItem->AddPopUpWindows(audioSettingsPopUp)->SetWindowName("Audio Settings");
		settingsGraphicsItem->AddPopUpWindows(graphicsSettingsPopUp)->SetWindowName("Graphics Settings");

		settingsAudioItem->SetWindowName("Audio");
		settingsGraphicsItem->SetWindowName("Graphics");
	}

	void EditorEngine::SetUpHelpTab()
	{
		std::shared_ptr<MainMenuItemParent> helpTab = AddMenuParent<MainMenuItemParent>();

		std::shared_ptr<MainMenuItemPopUp> cameraHelpButton = std::make_shared<MainMenuItemPopUp>("Camera Controls");
		std::shared_ptr<CameraControlsGuidePopUp> cameraControlsHelpPopUp = AddPopUpWindow<CameraControlsGuidePopUp>();

		helpTab->AddChild(cameraHelpButton);
		cameraHelpButton->AddPopUpWindows(cameraControlsHelpPopUp);

		helpTab->SetWindowName("Help");
		cameraControlsHelpPopUp->SetWindowName("Editor Camera");
	}
}
