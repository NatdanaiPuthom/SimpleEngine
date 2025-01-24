#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Editor.hpp"

#include "Editor/Core/Tabs/MenuTabWindow.hpp"
#include "Editor/Core/Tabs/MenuTabDefault.hpp"

#include "Editor/PopUps/Help/CameraControlsGuidePopUp.hpp"
#include "Editor/PopUps/Settings/CameraSettingsPopUp.hpp"
#include "Editor/PopUps/Settings/AudioSettingsPopUp.hpp"
#include "Editor/PopUps/Settings/GraphicsSettingsPopUp.hpp"

#include "Editor/PopUps/PostProcessPopUp.hpp"
#include "Editor/PopUps/DeferredPopUp.hpp"
#include "Editor/PopUps/Editor/EditorPopUp.hpp"
#include "Editor/PopUps/Editor/AssetBrowser.hpp"
#include "Editor/FlyScript/NodeScriptingWindow.hpp"

#include "Editor/Functions/SceneSettingFunctions.hpp"
#include "Editor/Functions/EditorCallbackFunctions.hpp"

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
		SetUpDefaultLayout();

		for (auto& menuTab : myMainMenuTabs)
		{
			menuTab->Init();
		}

		for (auto& window : myPopUpWindows)
		{
			window->Init();
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

		/*for (const std::shared_ptr<PopUp> popUp : myPopUpWindows)
		{
			popUp->Update();
		}*/
	}

	void EditorEngine::Render()
	{
		for (auto& tab : myMainMenuTabs)
		{
			tab->Render();
		}

		for (auto& popUp : myPopUpWindows)
		{
			if (popUp->IsActive())
			{
				popUp->Render();
			}
		}
	}

	void EditorEngine::SetUpDefaultLayout()
	{
		std::unique_ptr<MenuTabDefault> sceneTab = std::make_unique< MenuTabDefault>("Scene");
		std::unique_ptr<MenuTabWindow> windowsTab = std::make_unique< MenuTabWindow>("Windows");
		std::unique_ptr<MenuTabDefault> settingsTab = std::make_unique< MenuTabDefault>("Settings");
		std::unique_ptr<MenuTabDefault> helpTab = std::make_unique< MenuTabDefault>("Help");

		std::unique_ptr<MenuItemButton> sceneSaveButton = std::make_unique<MenuItemButton>("Save");
		std::unique_ptr<MenuItemSelectable> sceneLoadSelectable = std::make_unique<MenuItemSelectable>("Load");
		std::unique_ptr<MenuItemMenu> sceneCreateMenu = std::make_unique<MenuItemMenu>("Create");
		std::unique_ptr<MenuItemButton> sceneReloadButton = std::make_unique<MenuItemButton>("Reload");
		std::unique_ptr<MenuItemButton> sceneSetAsActiveButton = std::make_unique<MenuItemButton>("Set As Active");
		std::unique_ptr<MenuItemButton> sceneCreateNewButton = std::make_unique<MenuItemButton>("New");
		std::unique_ptr<MenuItemButton> sceneCreateCopyButton = std::make_unique<MenuItemButton>("Copy");

		std::unique_ptr<MenuItemPopUp> settingsAudioButton = std::make_unique<MenuItemPopUp>("Audio");
		std::unique_ptr<MenuItemPopUp> settingsCameraButton = std::make_unique<MenuItemPopUp>("Camera");
		std::unique_ptr<MenuItemPopUp> settingsGraphicsButton = std::make_unique<MenuItemPopUp>("Graphics");

		std::unique_ptr<MenuItemPopUp> helpCameraControlsPopUpButton = std::make_unique<MenuItemPopUp>("Camera Controls");

		std::unique_ptr<MenuItemPopUp> editorPopUpButton = std::make_unique<MenuItemPopUp>("Editor");
		std::unique_ptr<MenuItemPopUp> deferredPopUpButton = std::make_unique<MenuItemPopUp>("Deferred");
		std::unique_ptr<MenuItemPopUp> postProcessPopUpButton = std::make_unique<MenuItemPopUp>("PostProcess");
		std::unique_ptr<MenuItemPopUp> nodeScriptingPopUpButton = std::make_unique<MenuItemPopUp>("NodeScript");

		std::shared_ptr<AudioSettingsPopUp> audioSettingPopUp = std::make_shared<AudioSettingsPopUp>("Audio Settings");
		std::shared_ptr<CameraSettingsPopUp> cameraSettingPopUp = std::make_shared<CameraSettingsPopUp>("Camera Settings");
		std::shared_ptr<GraphicsSettingsPopUp> graphicsSettingPopUp = std::make_shared<GraphicsSettingsPopUp>("Graphics Settings");
		std::shared_ptr<CameraControlsGuidePopUp> cameraHelpPopUp = std::make_shared<CameraControlsGuidePopUp>("Editor Camera Control");
		std::shared_ptr<DeferredPopUp> deferredPopUp = std::make_shared<DeferredPopUp>("Deferred Window");
		std::shared_ptr<PostProcessPopUp> postProcessPopUp = std::make_shared<PostProcessPopUp>("PostProcess Window");
		std::shared_ptr<EditorPopUp> editorPopUp = std::make_shared<EditorPopUp>("Editor Window");
		std::shared_ptr<AssetBrowserPopUp> assetBrowserPopUp = std::make_shared<AssetBrowserPopUp>("AssetBrowser Window");
		std::shared_ptr<NodeScriptingWindow> nodeScriptingPopUp = std::make_shared<NodeScriptingWindow>("NodeScripting Window");

		sceneCreateNewButton->SetCallback(SceneSettingsFunction::CreateNew());
		sceneCreateCopyButton->SetCallback(SceneSettingsFunction::CreateCopy());

		sceneSaveButton->SetCallback(SceneSettingsFunction::Save());
		sceneReloadButton->SetCallback(SceneSettingsFunction::Reload());
		sceneSetAsActiveButton->SetCallback(SceneSettingsFunction::SetAsActive());

		{
			const std::vector<std::string> sceneNames = SimpleUtilities::FileManager::GetFileNamesFromDirectory(SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_SCENES));

			sceneLoadSelectable->SetStrings(sceneNames);
			sceneLoadSelectable->SetCallback(SceneSettingsFunction::Load());
		}

		sceneCreateMenu->AddChild(std::move(sceneCreateNewButton));
		sceneCreateMenu->AddChild(std::move(sceneCreateCopyButton));

		helpCameraControlsPopUpButton->SetCallback(std::move(EditorCallbacks::SetPopUpActive(cameraHelpPopUp, &helpCameraControlsPopUpButton->GetIsActiveRef())));
		settingsAudioButton->SetCallback(std::move(EditorCallbacks::SetPopUpActive(audioSettingPopUp, &settingsAudioButton->GetIsActiveRef())));
		settingsCameraButton->SetCallback(std::move(EditorCallbacks::SetPopUpActive(cameraSettingPopUp, &settingsCameraButton->GetIsActiveRef())));
		settingsGraphicsButton->SetCallback(std::move(EditorCallbacks::SetPopUpActive(graphicsSettingPopUp, &settingsGraphicsButton->GetIsActiveRef())));
		deferredPopUpButton->SetCallback(std::move(EditorCallbacks::SetPopUpActive(deferredPopUp, &deferredPopUpButton->GetIsActiveRef())));
		postProcessPopUpButton->SetCallback(std::move(EditorCallbacks::SetPopUpActive(postProcessPopUp, &postProcessPopUpButton->GetIsActiveRef())));
		editorPopUpButton->SetCallback(std::move(EditorCallbacks::SetPopUpActive(editorPopUp, &editorPopUpButton->GetIsActiveRef())));
		editorPopUpButton->SetCallback(std::move(EditorCallbacks::SetPopUpActive(assetBrowserPopUp, &editorPopUpButton->GetIsActiveRef())));
		nodeScriptingPopUpButton->SetCallback(std::move(EditorCallbacks::SetPopUpActive(nodeScriptingPopUp, &nodeScriptingPopUpButton->GetIsActiveRef())));

		{ //TO-DO(v11.4.5): Temp should be refactor
			editorPopUpButton->SetIsActive(true);
			editorPopUpButton->Invoke();
		}

		sceneTab->AddButton(std::move(sceneSaveButton));
		sceneTab->AddSelectable(std::move(sceneLoadSelectable));
		sceneTab->AddMenu(std::move(sceneCreateMenu));
		sceneTab->AddButton(std::move(sceneReloadButton));
		sceneTab->AddButton(std::move(sceneSetAsActiveButton));

		windowsTab->AddPopUp(std::move(editorPopUpButton));
		windowsTab->AddPopUp(std::move(deferredPopUpButton));
		windowsTab->AddPopUp(std::move(postProcessPopUpButton));
		windowsTab->AddPopUp(std::move(nodeScriptingPopUpButton));

		settingsTab->AddPopUp(std::move(settingsAudioButton));
		settingsTab->AddPopUp(std::move(settingsCameraButton));
		settingsTab->AddPopUp(std::move(settingsGraphicsButton));

		helpTab->AddPopUp(std::move(helpCameraControlsPopUpButton));

		myMainMenuTabs.push_back(std::move(sceneTab));
		myMainMenuTabs.push_back(std::move(windowsTab));
		myMainMenuTabs.push_back(std::move(settingsTab));
		myMainMenuTabs.push_back(std::move(helpTab));

		myPopUpWindows.push_back(audioSettingPopUp);
		myPopUpWindows.push_back(cameraSettingPopUp);
		myPopUpWindows.push_back(graphicsSettingPopUp);
		myPopUpWindows.push_back(cameraHelpPopUp);
		myPopUpWindows.push_back(deferredPopUp);
		myPopUpWindows.push_back(postProcessPopUp);
		myPopUpWindows.push_back(editorPopUp);
		myPopUpWindows.push_back(assetBrowserPopUp);
		myPopUpWindows.push_back(nodeScriptingPopUp);
	}
}
