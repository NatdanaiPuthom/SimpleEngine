#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Editor.hpp"

#include "MainSingleton/MainSingleton.hpp"

#include "Editor/PopUps/Help/CameraControlsGuidePopUp.hpp"
#include "Editor/PopUps/Settings/CameraSettingsPopUp.hpp"
#include "Editor/PopUps/Settings/AudioSettingsPopUp.hpp"
#include "Editor/PopUps/Settings/GraphicsSettingsPopUp.hpp"

#include "Editor/MainMenuTabs/Files.hpp"
#include "Editor/MainMenuTabs/Settings.hpp"
#include "Editor/MainMenuTabs/Help.hpp"

#include "Editor/MainMenuItems/Settings/AudioItem.hpp"
#include "Editor/MainMenuItems/Settings/CameraSettingsItem.hpp"
#include "Editor/MainMenuItems/Settings/GraphicsSettingsItem.hpp"
#include "Editor/MainMenuItems/Help/CameraHelpItem.hpp"

namespace Editor
{
	size_t EditorEngine::mySelectedEntityID = static_cast<size_t>(-1);

	EditorEngine::EditorEngine()
	{
		/*
		AddTool(std::make_unique<SceneMenuBar>());
		AddTool(std::make_unique<MainMenuBar>());
		AddTool(std::make_unique<SettingsMenuBar>());
		AddTool(std::make_unique<PlayMenuBar>());*/
	}

	EditorEngine::~EditorEngine()
	{
	}

	void EditorEngine::Init()
	{
		std::shared_ptr<Files> fileTab = AddMenuTab<Files>();
		std::shared_ptr<Settings> settingsTab = AddMenuTab<Settings>();
		std::shared_ptr<Help> helpTab = AddMenuTab<Help>();

		std::shared_ptr<CameraControlsGuidePopUp> cameraControlsHelpPopUp = AddPopUpWindow<CameraControlsGuidePopUp>();
		std::shared_ptr<CameraSettingsPopUp> cameraSettingsPopUp = AddPopUpWindow<CameraSettingsPopUp>();
		std::shared_ptr<AudioSettingsPopUp> audioSettingsPopUp = AddPopUpWindow<AudioSettingsPopUp>();
		std::shared_ptr<GraphicsSettingsPopUp> graphicsSettingsPopUp = AddPopUpWindow<GraphicsSettingsPopUp>();

		std::shared_ptr<AudioItem> settingsAudioItem = settingsTab->AddChildren<AudioItem>();
		std::shared_ptr<CameraHelpItem> settingsCameraItem = settingsTab->AddChildren<CameraHelpItem>();
		std::shared_ptr<GraphicsSettingsItem> settingsGraphicsItem = settingsTab->AddChildren<GraphicsSettingsItem>();
		std::shared_ptr<CameraHelpItem> helpCameraItem = helpTab->AddChildren<CameraHelpItem>();

		helpCameraItem->AddPopUpWindows(cameraControlsHelpPopUp);
		settingsCameraItem->AddPopUpWindows(cameraSettingsPopUp)->SetWindowName("Camera Settings");
		settingsAudioItem->AddPopUpWindows(audioSettingsPopUp)->SetWindowName("Audio Settings");
		settingsGraphicsItem->AddPopUpWindows(graphicsSettingsPopUp)->SetWindowName("Graphics Settings");

		cameraControlsHelpPopUp->SetWindowName("Editor Camera");
		helpCameraItem->SetWindowName("Camera Controls");
		settingsCameraItem->SetWindowName("Camera");
		fileTab->SetWindowName("File");
		settingsAudioItem->SetWindowName("Audio");
		settingsGraphicsItem->SetWindowName("Graphics");

		for (const std::shared_ptr<PopUp> popUpWindow : myPopUpWindows)
		{
			popUpWindow->Init();
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

		for (const std::shared_ptr<MainMenuTab> window : myMainMenuTabs)
		{
			window->Update();
		}

		for (const std::shared_ptr<PopUp> window : myPopUpWindows)
		{
			window->Update();
		}
	}

	void EditorEngine::Render()
	{
		for (const std::shared_ptr<MainMenuTab> menuTab : myMainMenuTabs)
		{
			if (menuTab->IsActive())
			{
				menuTab->Render();
			}
		}

		for (const std::shared_ptr<PopUp> window : myPopUpWindows)
		{
			if (window->IsActive())
			{
				window->Render();
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
}
