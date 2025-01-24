#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Editor.hpp"

#include "MainSingleton/MainSingleton.hpp"


#include "Editor/PopUps/Help/CameraControlsGuidePopUp.hpp"
#include "Editor/PopUps/Settings/CameraSettingsPopUp.hpp"
#include "Editor/PopUps/Settings/AudioSettingsPopUp.hpp"
#include "Editor/PopUps/Settings/GraphicsSettingsPopUp.hpp"

#include "Editor/PopUps/PostProcessPopUp.hpp"
#include "Editor/PopUps/DeferredPopUp.hpp"

#include "Engine/ImGui/ImGuiEngine.hpp" //TempPlayMenuBar

#include "Editor/Core/Tabs/MenuItemWindow.hpp"
#include "Editor/Core/Tabs/MenuItemTab.hpp"

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

	static std::function<void()> SetPopUpActive(std::shared_ptr<PopUp> aPopUp, bool* aBoolean)
	{
		return [=]() -> void
			{
				aPopUp->SetActive(*aBoolean);
				std::cout << aPopUp->GetWindowName() << " was set " << *aBoolean << std::endl;
			};
	}
}

namespace Editor
{
	std::vector<std::unique_ptr<MainMenuTabBase>> globalMainMenuTabs;
	std::vector<std::shared_ptr<PopUp>> globalPopUps;
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
		std::unique_ptr<MenuTabDefault> scene = std::make_unique< MenuTabDefault>("Scene");

		auto saveButton = scene->AddButton("Save", IWasClicked); saveButton;
		auto newMenu = scene->AddMenu("A New Menu");

		std::unique_ptr<MenuItemButton> button = std::make_unique<MenuItemButton>("New Child Button", IWasClicked);
		std::unique_ptr<MenuItemButton> subMenuButton = std::make_unique<MenuItemButton>("New Sub Child Button", IWasClicked);
		std::unique_ptr<MenuItemMenu> subMenu = std::make_unique<MenuItemMenu>("New Child Menu Button");

		auto selectable = scene->AddSelectable("Selectable", SelectableClick); selectable;

		std::shared_ptr<AudioSettingsPopUp> audioSettingPopUP = std::make_shared<AudioSettingsPopUp>("Audio");
		std::shared_ptr<CameraSettingsPopUp> cameraSettingPopUp = std::make_shared<CameraSettingsPopUp>("Camera");
		std::shared_ptr<GraphicsSettingsPopUp> graphicsSettingPopUP = std::make_shared<GraphicsSettingsPopUp>("Graphics");

		globalPopUps.push_back(audioSettingPopUP);
		globalPopUps.push_back(cameraSettingPopUp);
		globalPopUps.push_back(graphicsSettingPopUP);

		std::unique_ptr<MenuItemPopUp> popUpTest = std::make_unique<MenuItemPopUp>("Pop Up!");

		auto testtt = SetPopUpActive(audioSettingPopUP, &popUpTest->myTestBool);

		popUpTest->SetCallback(std::move(testtt));

		scene->AddPopUp(std::move(popUpTest));

		std::vector<std::string> strings;
		strings.push_back("Emil");
		strings.push_back("Erico");
		strings.push_back("Test");

		selectable->SetStrings(strings);

		auto newButton = newMenu->AddChild(std::move(button)); newButton;
		auto newMenuButton = newMenu->AddChild(std::move(subMenu)); newMenuButton;
		newMenuButton->AddChild(std::move(subMenuButton));

		globalMainMenuTabs.push_back(std::move(scene));

		std::unique_ptr<MenuItemPopUp> windowPopUp1 = std::make_unique<MenuItemPopUp>("Window1!!");
		std::unique_ptr<MenuItemPopUp> windowPopUp2 = std::make_unique<MenuItemPopUp>("Window2!!");

		std::unique_ptr<MenuTabWindow> window = std::make_unique< MenuTabWindow>("Windows");

		auto windowCallback1 = SetPopUpActive(cameraSettingPopUp, &windowPopUp1->myTestBool);
		auto windowCallback2 = SetPopUpActive(graphicsSettingPopUP, &windowPopUp1->myTestBool);
		windowPopUp1->SetCallback(std::move(windowCallback1));
		windowPopUp1->SetCallback(std::move(windowCallback2));

		auto windowCallback3 = SetPopUpActive(audioSettingPopUP, &windowPopUp2->myTestBool);
		windowPopUp2->SetCallback(std::move(windowCallback3));

		window->AddPopUp(std::move(windowPopUp1));
		window->AddPopUp(std::move(windowPopUp2));

		window->Init();

		globalMainMenuTabs.push_back(std::move(window));

		for (auto& p : globalPopUps)
		{
			p->Init();
		}

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
		for (auto& tab : globalMainMenuTabs)
		{
			tab->Render();
		}

		for (auto& popUp : globalPopUps)
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
