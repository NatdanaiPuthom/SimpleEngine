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

#include "Engine/ImGui/ImGuiEngine.hpp" //TempPlayMenuBar

#include "Engine/Debugger/Console/Console.hpp"
#include "Engine/SimpleUtilities/FileManager/FileManager.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace Editor
{
	static void IWasClicked()
	{
		std::cout << "i was click" << std::endl;
	}

	static void SelectableClick(const std::string& aString)
	{
		std::cout << aString << " was clicked!" << std::endl;
	}

	static void IWasClickedWithParameters(int value)
	{
		std::cout << "i was click with value: " << value << std::endl;
	}


	class EditorCallbacks
	{
	public:
		static std::function<void()> SetPopUpActive(std::shared_ptr<PopUp> aPopUp, bool* aBoolean)
		{
			return [=]() -> void
				{
					aPopUp->SetActive(*aBoolean);
					std::cout << aPopUp->GetWindowName() << " was set " << *aBoolean << std::endl;
				};
		}
	};

	class SceneSettingsFunction
	{
	public:
		static std::function<void()> Save()
		{
			return []() -> void
				{
					const Simpleton::SceneInfo* sceneInfo = MainSingleton::GetSceneManager().GetCurrentSceneInfo();
					ECS::EntityComponentSystem& ecs = MainSingleton::GetSceneManager().GetCurrentECS();
					ECS::EntityComponentSystem::SaveData(ecs, sceneInfo->relativePath);

					Simple::Console::Print("Scene ", Simple::ConsoleTextColor::White, false);
					Simple::Console::Print(sceneInfo->name.c_str(), Simple::ConsoleTextColor::Green, false);
					Simple::Console::Print(" has been saved!", Simple::ConsoleTextColor::White, true);
				};
		}

		static std::function<void(const std::string&)> Load()
		{
			return [](const std::string& aString) -> void
				{
					const std::string scenePath = std::string(SIMPLE_DIR_SCENES) + "\\" + aString;
					MainSingleton::GetSceneManager().ChangeScene(scenePath);
					Simple::Console::Print("Loaded scene ", Simple::ConsoleTextColor::White, false);
					Simple::Console::Print(aString.c_str(), Simple::ConsoleTextColor::Green, false);
					Simple::Console::Print("!", Simple::ConsoleTextColor::White, true);
				};
		}

		static std::function<void()> CreateNew()
		{
			return []() -> void
				{
					Simpleton::SceneManager& sceneManager = MainSingleton::GetSceneManager();

					const std::string absolutePath = SimpleUtilities::GetAbsolutePath(SimpleUtilities::AppendCounterIfAlreadyExist(std::string(SIMPLE_DIR_SCENES) + "\\" + std::string(SIMPLE_FILENAME_NEWSCENE)));
					const std::string relativePath = SimpleUtilities::ConvertAbsolutePathToRelativePath(absolutePath);

					sceneManager.CreateNewScene(absolutePath);
					sceneManager.ChangeScene(relativePath);

					Simple::Console::Print("New scene ", Simple::ConsoleTextColor::White, false);
					Simple::Console::Print(sceneManager.GetCurrentSceneInfo()->name.c_str(), Simple::ConsoleTextColor::Green, false);
					Simple::Console::Print(" has been created!", Simple::ConsoleTextColor::White, true);
				};
		}

		static std::function<void()> CreateCopy()
		{
			return []() -> void
				{
					Simpleton::SceneManager& sceneManager = MainSingleton::GetSceneManager();

					const std::string absolutePath = sceneManager.GetCurrentSceneInfo()->absolutePath;
					const std::string newCopyName = SimpleUtilities::AppendStringBeforeDot("_Copy", absolutePath);
					const std::string newFileName = SimpleUtilities::AppendCounterIfAlreadyExist(newCopyName);
					const std::string relativePath = SimpleUtilities::ConvertAbsolutePathToRelativePath(newFileName);

					std::filesystem::copy_file(absolutePath, newFileName, std::filesystem::copy_options::overwrite_existing);
					sceneManager.ChangeScene(relativePath);

					Simple::Console::Print("New scene ", Simple::ConsoleTextColor::White, false);
					Simple::Console::Print(sceneManager.GetCurrentSceneInfo()->name.c_str(), Simple::ConsoleTextColor::Green, false);
					Simple::Console::Print(" has been created!", Simple::ConsoleTextColor::White, true);
				};
		}

		static std::function<void()> Reload()
		{
			return []() -> void
				{
					Simpleton::SceneManager& sceneManager = MainSingleton::GetSceneManager();
					sceneManager.ReloadSceneFromFile(sceneManager.GetCurrentSceneInfo()->relativePath);

					Simple::Console::Print("Scene ", Simple::ConsoleTextColor::White, false);
					Simple::Console::Print(sceneManager.GetCurrentSceneInfo()->name.c_str(), Simple::ConsoleTextColor::Green, false);
					Simple::Console::Print(" has been reloaded!", Simple::ConsoleTextColor::White, true);
				};
		}

		static std::function<void()> SetAsActive()
		{
			return []() -> void
				{
					const Simpleton::SceneInfo* sceneInfo = MainSingleton::GetSceneManager().GetCurrentSceneInfo();
					nlohmann::json jsonData = SimpleUtilities::FileManager::GetDataAsJson(SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_GAME));
					jsonData["Game_Settings"]["Start_Scene_RelativePath"] = sceneInfo->relativePath;

					std::ofstream writeFile(SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_GAME));
					assert(writeFile.is_open() && "Failed to open the file");

					writeFile << jsonData;
					writeFile.close();

					Simple::Console::Print("Scene ", Simple::ConsoleTextColor::White, false);
					Simple::Console::Print(sceneInfo->name.c_str(), Simple::ConsoleTextColor::Green, false);
					Simple::Console::Print(" has been set as start!", Simple::ConsoleTextColor::White, true);
				};
		}
	};
}

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

		std::shared_ptr<AudioSettingsPopUp> audioSettingPopUp = std::make_shared<AudioSettingsPopUp>("Audio Settings");
		std::shared_ptr<CameraSettingsPopUp> cameraSettingPopUp = std::make_shared<CameraSettingsPopUp>("Camera Settings");
		std::shared_ptr<GraphicsSettingsPopUp> graphicsSettingPopUp = std::make_shared<GraphicsSettingsPopUp>("Graphics Settings");
		std::shared_ptr<CameraControlsGuidePopUp> cameraHelpPopUp = std::make_shared<CameraControlsGuidePopUp>("Editor Camera Control");

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

		{
			auto popUpCallback = EditorCallbacks::SetPopUpActive(cameraHelpPopUp, &helpCameraControlsPopUpButton->myTestBool);
			helpCameraControlsPopUpButton->SetCallback(std::move(popUpCallback));
		}

		{
			auto popUpCallback = EditorCallbacks::SetPopUpActive(audioSettingPopUp, &settingsAudioButton->myTestBool);
			settingsAudioButton->SetCallback(std::move(popUpCallback));
		}

		{
			auto popUpCallback = EditorCallbacks::SetPopUpActive(cameraSettingPopUp, &settingsCameraButton->myTestBool);
			settingsCameraButton->SetCallback(std::move(popUpCallback));
		}

		{
			auto popUpCallback = EditorCallbacks::SetPopUpActive(graphicsSettingPopUp, &settingsGraphicsButton->myTestBool);
			settingsGraphicsButton->SetCallback(std::move(popUpCallback));
		}

		sceneTab->AddButton(std::move(sceneSaveButton));
		sceneTab->AddSelectable(std::move(sceneLoadSelectable));
		sceneTab->AddMenu(std::move(sceneCreateMenu));
		sceneTab->AddButton(std::move(sceneReloadButton));
		sceneTab->AddButton(std::move(sceneSetAsActiveButton));

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

		for (auto& t : myMainMenuTabs)
		{
			t->Init();
		}

		for (auto& p : myPopUpWindows)
		{
			p->Init();
		}

		//std::unique_ptr<MenuItemPopUp> windowPopUp1 = std::make_unique<MenuItemPopUp>("Window1!!");
		//std::unique_ptr<MenuItemPopUp> windowPopUp2 = std::make_unique<MenuItemPopUp>("Window2!!");

		//std::unique_ptr<MenuTabWindow> window = std::make_unique< MenuTabWindow>("Windows");

		//auto windowCallback1 = SetPopUpActive(cameraSettingPopUp, &windowPopUp1->myTestBool);
		//auto windowCallback2 = SetPopUpActive(graphicsSettingPopUP, &windowPopUp1->myTestBool);
		//windowPopUp1->SetCallback(std::move(windowCallback1));
		//windowPopUp1->SetCallback(std::move(windowCallback2));

		//auto windowCallback3 = SetPopUpActive(audioSettingPopUP, &windowPopUp2->myTestBool);
		//windowPopUp2->SetCallback(std::move(windowCallback3));
	}

	void EditorEngine::Update()
	{
		/*if (MainSingleton::GetSceneManager().GetIsPlaying() == false)
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

		//TempPlayMenuBar();

		//{	//Render Orientation Cube  
		//	//TO-DO(v11.4.4): Make own class for this
  //          const Graphics::Camera* camera = Global::GetGraphicsEngine()->GetCurrentCamera();  
  //          Math::Matrix4x4f view = camera->GetViewMatrix();  
  //          ImVec2 windowPos = ImGui::GetWindowPos();  
  //          ImGuizmo::ViewManipulate(&view(1, 1), 16, ImVec2(windowPos.x + 775, windowPos.y + 375), ImVec2(64, 64), 0x00000000);
		//}
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
