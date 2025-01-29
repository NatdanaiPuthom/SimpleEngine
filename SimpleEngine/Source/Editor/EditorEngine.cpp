#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/EditorEngine.hpp"

#include "Editor/Core/Tabs/MenuTabWindow.hpp"
#include "Editor/Core/Tabs/MenuTabDefault.hpp"

#include "Editor/PopUps/Help/CameraControlsGuidePopUp.hpp"
#include "Editor/PopUps/Settings/CameraSettingsPopUp.hpp"
#include "Editor/PopUps/Settings/AudioSettingsPopUp.hpp"
#include "Editor/PopUps/Settings/GraphicsSettingsPopUp.hpp"

#include "Editor/PopUps/PostProcessPopUp.hpp"
#include "Editor/PopUps/DeferredPopUp.hpp"
#include "Editor/PopUps/Editor/SceneHierachyPopUp.hpp"
#include "Editor/PopUps/Editor/SceneInspectorPopUp.hpp"
#include "Editor/PopUps/Editor/AssetBrowserPopUp.hpp"
#include "Editor/PopUps/Editor/SceneWindowPopUp.hpp"
#include "Editor/PopUps/Editor/EditorPopUp.hpp"
#include "Editor/PopUps/Editor/AssetBrowser.hpp"
#include "Editor/FlyScript/NodeScriptingWindow.hpp"

#include "Editor/Functions/SceneSettingFunctions.hpp"
#include "Editor/Functions/EditorCallbackFunctions.hpp"

#include "Editor/EditorProxy.hpp"

namespace Editor
{
	EditorEngine::EditorEngine()
	{
	}

	EditorEngine::~EditorEngine()
	{
	}

	void EditorEngine::Init()
	{
		EditorProxy::myEditorEngine = this;

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

		const Simpleton::InputManager& inputManager = MainSingleton::GetInputManager();
		if (inputManager.IsKeyPressed(VK_F5))
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

		if (inputManager.IsKeyHeld(VK_CONTROL))
		{
			if (inputManager.IsKeyPressed('Z'))
			{
				myCommandTracker.UndoCommand();
			}
			else if (inputManager.IsKeyPressed('Y'))
			{
				myCommandTracker.RedoCommand();
			}
		}

		//TO-DO(v12.0.0): Should fix as currently updating IsFocused for all windows not working
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


	CommandTracker& EditorEngine::GetCommandTracker()
	{
		return myCommandTracker;
	}

	void EditorEngine::SetUpDefaultLayout()
	{
		MenuTabDefault* sceneTab = AddMenuTab<MenuTabDefault>("Scene");
		MenuTabWindow* windowsTab = AddMenuTab<MenuTabWindow>("Windows");
		MenuTabDefault* settingsTab = AddMenuTab<MenuTabDefault>("Settings");
		MenuTabDefault* helpTab = AddMenuTab<MenuTabDefault>("Help");

		MenuItemButton* sceneSaveButton = sceneTab->AddButton("Save");
		MenuItemSelectable* sceneLoadSelectable = sceneTab->AddSelectable("Load");
		MenuItemMenu* sceneCreateMenu = sceneTab->AddMenu("Create");
		MenuItemButton* sceneReloadButton = sceneTab->AddButton("Reload");
		MenuItemButton* sceneSetAsActiveButton = sceneTab->AddButton("Set As Active");
	
		MenuItemButton* sceneCreateNewButton = sceneCreateMenu->AddChild<MenuItemButton>("New");
		MenuItemButton* sceneCreateCopyButton = sceneCreateMenu->AddChild<MenuItemButton>("Copy");

		MenuItemPopUp* settingsAudioButton = settingsTab->AddPopUp("Audio");
		MenuItemPopUp* settingsCameraButton = settingsTab->AddPopUp("Camera");
		MenuItemPopUp* settingsGraphicsButton = settingsTab->AddPopUp("Graphics");

		MenuItemPopUp* helpCameraControlsPopUpButton = helpTab->AddPopUp("Camera Controls");

		MenuItemPopUp* editorPopUpButton = windowsTab->AddPopUp("Editor");
		MenuItemPopUp* deferredPopUpButton = windowsTab->AddPopUp("Deferred");
		MenuItemPopUp* postProcessPopUpButton = windowsTab->AddPopUp("PostProcess");
		MenuItemPopUp* nodeScriptingPopUpButton = windowsTab->AddPopUp("NodeScript"); nodeScriptingPopUpButton;

		std::shared_ptr<AudioSettingsPopUp> audioSettingPopUp = AddPopUpWindow<AudioSettingsPopUp>("Audio Settings");
		std::shared_ptr<CameraSettingsPopUp> cameraSettingPopUp = AddPopUpWindow<CameraSettingsPopUp>("Camera Settings");
		std::shared_ptr<GraphicsSettingsPopUp> graphicsSettingPopUp = AddPopUpWindow<GraphicsSettingsPopUp>("Graphics Settings");
		std::shared_ptr<CameraControlsGuidePopUp> cameraHelpPopUp = AddPopUpWindow<CameraControlsGuidePopUp>("Editor Camera Control");
		std::shared_ptr<DeferredPopUp> deferredPopUp = AddPopUpWindow<DeferredPopUp>("Deferred Window");
		std::shared_ptr<PostProcessPopUp> postProcessPopUp = AddPopUpWindow<PostProcessPopUp>("PostProcess Window");
		std::shared_ptr<SceneHierachyPopUp> sceneHierarchyPopUp = AddPopUpWindow<SceneHierachyPopUp>("Hierarchy", &myCommandTracker);
		std::shared_ptr<SceneInspectorPopUp> sceneInspectorPopUp = AddPopUpWindow<SceneInspectorPopUp>("Inspector");
		std::shared_ptr<AssetBrowserPopUp2> assetBrowserPopUp2 = AddPopUpWindow<AssetBrowserPopUp2>("Asset Browser");
		std::shared_ptr<SceneWindowPopUp> sceneWindowPopUp = AddPopUpWindow<SceneWindowPopUp>("Scene");

		//std::shared_ptr<EditorPopUp> editorPopUp = std::make_shared<EditorPopUp>("Editor Window", &myCommandTracker);

		{ //TO-DO(v12.0.0): Temp should be refactor
			//myPopUpWindows.push_back(editorPopUp);
		}

		//std::shared_ptr<AssetBrowserPopUp> assetBrowserPopUp = AddPopUpWindow<AssetBrowserPopUp>("AssetBrowser Window");
		//std::shared_ptr<NodeScriptingWindow> nodeScriptingPopUp = AddPopUpWindow<NodeScriptingWindow>("NodeScripting Window");

		{	//TO-DO(v12.0.0): Temp should be refactor
			//assetBrowserPopUp->myNodeScriptingWindow = nodeScriptingPopUp.get();
			//assetBrowserPopUp->myNodeScriptParentTab = windowsTab;
			//assetBrowserPopUp->myNodeScriptButton = nodeScriptingPopUpButton;
		}

		{
			const std::vector<std::string> sceneNames = SimpleUtilities::FileManager::GetFileNamesFromDirectory(SimpleUtilities::GetAbsolutePath(SIMPLE_DIR_SCENES));
			sceneLoadSelectable->SetStrings(sceneNames);
		}

		sceneSaveButton->SetCallback(SceneSettingsFunction::Save());
		sceneLoadSelectable->SetCallback(SceneSettingsFunction::Load());
		sceneCreateNewButton->SetCallback(SceneSettingsFunction::CreateNew());
		sceneCreateCopyButton->SetCallback(SceneSettingsFunction::CreateCopy());
		sceneReloadButton->SetCallback(SceneSettingsFunction::Reload());
		sceneSetAsActiveButton->SetCallback(SceneSettingsFunction::SetAsActive());

		helpCameraControlsPopUpButton->SetCallback(std::move(EditorCallbacks::SetPopUpActive(cameraHelpPopUp, &helpCameraControlsPopUpButton->GetIsActiveRef())));
		settingsAudioButton->SetCallback(std::move(EditorCallbacks::SetPopUpActive(audioSettingPopUp, &settingsAudioButton->GetIsActiveRef())));
		settingsCameraButton->SetCallback(std::move(EditorCallbacks::SetPopUpActive(cameraSettingPopUp, &settingsCameraButton->GetIsActiveRef())));
		settingsGraphicsButton->SetCallback(std::move(EditorCallbacks::SetPopUpActive(graphicsSettingPopUp, &settingsGraphicsButton->GetIsActiveRef())));
		deferredPopUpButton->SetCallback(std::move(EditorCallbacks::SetPopUpActive(deferredPopUp, &deferredPopUpButton->GetIsActiveRef())));
		postProcessPopUpButton->SetCallback(std::move(EditorCallbacks::SetPopUpActive(postProcessPopUp, &postProcessPopUpButton->GetIsActiveRef())));
		//editorPopUpButton->SetCallback(std::move(EditorCallbacks::SetPopUpActive(editorPopUp, &editorPopUpButton->GetIsActiveRef())));
		//editorPopUpButton->SetCallback(std::move(EditorCallbacks::SetPopUpActive(assetBrowserPopUp, &editorPopUpButton->GetIsActiveRef())));
		editorPopUpButton->SetCallback(std::move(EditorCallbacks::SetPopUpActive(sceneHierarchyPopUp, &editorPopUpButton->GetIsActiveRef())));
		editorPopUpButton->SetCallback(std::move(EditorCallbacks::SetPopUpActive(sceneInspectorPopUp, &editorPopUpButton->GetIsActiveRef())));
		editorPopUpButton->SetCallback(std::move(EditorCallbacks::SetPopUpActive(assetBrowserPopUp2, &editorPopUpButton->GetIsActiveRef())));
		editorPopUpButton->SetCallback(std::move(EditorCallbacks::SetPopUpActive(sceneWindowPopUp, &editorPopUpButton->GetIsActiveRef())));

		//nodeScriptingPopUpButton->SetCallback(std::move(EditorCallbacks::SetPopUpActive(nodeScriptingPopUp, &nodeScriptingPopUpButton->GetIsActiveRef())));

		{ //TO-DO(v11.4.5): Temp should be refactor
			editorPopUpButton->SetIsActive(true);
			editorPopUpButton->Invoke();
		}
	}
}
