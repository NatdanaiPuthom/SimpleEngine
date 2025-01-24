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
#include "Editor/PopUps/DeferredPopUp.hpp"

#include "Engine/ImGui/ImGuiEngine.hpp" //TempPlayMenuBar

#include <functional>

namespace Editor
{
	class MenuItemTest
	{
	public:
		MenuItemTest(const char* aName) : myName(aName) {}
		virtual ~MenuItemTest() = default;

		virtual void Render() = 0;

		const std::string& GetName() const { return myName; }

	protected:
		std::string myName;
	};

	template <typename T>
	concept DerivedFromMenuItemTest = std::is_base_of_v<MenuItemTest, T>&& std::is_class_v<T>;

	class MenuItemButton final : public MenuItemTest
	{
	public:
		MenuItemButton(const char* aName, std::function<void()> aCallback = nullptr) : MenuItemTest(aName)
			, myCallback(std::move(aCallback))
		{
		}

		void SetCallback(std::function<void()> aCallback)
		{
			myCallback = aCallback;
		}

		void Render() override final
		{
			if (ImGui::MenuItem(myName.c_str()) && myCallback)
			{
				myCallback();
			}
		}

	private:
		std::function<void()> myCallback;
	};

	class MenuItemPopUpTest final : public MenuItemTest
	{
	public:
		MenuItemPopUpTest(const char* aName, std::function<void()> aCallback = nullptr) : MenuItemTest(aName)
		{

		}

		void Render() override final
		{
			if (ImGui::MenuItem(myName.c_str(), nullptr, &myTestBool) && !myCallback.empty() && myCallback.front())
			{
				for (auto& callback : myCallback)
				{
					callback();
				}
			}
		}

		void SetCallback(std::function<void()> aCallback)
		{
			myCallback.push_back(std::move(aCallback));
		}

		void Invoke()
		{
			if (!myCallback.empty() && myCallback.front())
			{
				for (auto& callback : myCallback)
				{
					callback();
				}
			}
		}

		bool myTestBool = false;

	private:
		std::vector< std::function<void()>> myCallback;
	};

	class MenuItemMenu final : public MenuItemTest
	{
	public:
		MenuItemMenu(const char* aName) : MenuItemTest(aName)
		{
		}

		template<DerivedFromMenuItemTest T>
		T* AddChild(std::unique_ptr<T> aChild)
		{
			T* ptr = aChild.get();
			myChildren.push_back(std::move(aChild));
			return ptr;
		}

		void Render() override final
		{
			if (ImGui::BeginMenu(myName.c_str()))
			{
				for (const auto& child : myChildren)
				{
					child->Render();
				}

				ImGui::EndMenu();
			}
		}

	private:
		std::vector<std::unique_ptr<MenuItemTest>> myChildren;
	};

	class MenuItemSelectable final : public MenuItemTest
	{
	public:
		MenuItemSelectable(const char* aName, std::function<void(const std::string&)> aCallback) : MenuItemTest(aName)
			, myCallback(std::move(aCallback))
		{
		}

		void Render() override final
		{
			if (ImGui::BeginMenu(myName.c_str()))
			{
				for (const auto& name : myStrings)
				{
					if (ImGui::Selectable(name.c_str()) && myCallback)
					{
						myCallback(name);
					}
				}

				ImGui::EndMenu();
			}
		}

		void SetStrings(const std::vector<std::string> aNewStrings)
		{
			myStrings = aNewStrings;

		}

	private:
		std::function<void(const std::string&)> myCallback;
		std::vector<std::string> myStrings;
	};
}

namespace Editor
{
	static void IWasClicked()
	{
		std::cout << "i was click" << std::endl;
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

	class TestSeletectable
	{
	public:
		static void SelectableClick(const std::string& aString)
		{
			std::cout << aString << " was clicked!" << std::endl;
		}


	};

	class MainMenuTabTestBase
	{
	public:
		MainMenuTabTestBase(const char* aName) : myName(aName) {}

		virtual ~MainMenuTabTestBase() = default;
		virtual void Init() {};
		virtual void Render() = 0;

	protected:
		std::string myName;
	};

	class MenuWindowTest final : public MainMenuTabTestBase
	{
	public:
		MenuWindowTest(const char* aName) : MainMenuTabTestBase(aName) {}

		MenuItemPopUpTest* AddPopUp(std::unique_ptr<MenuItemPopUpTest> aButton)
		{
			MenuItemPopUpTest* buttonPointer = aButton.get();
			myButtons.push_back(std::move(aButton));
			return buttonPointer;
		}

		void Init() override final
		{
		}

		void Render() override final
		{
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu(myName.c_str()))
				{
					size_t currentButtonIndex = 0;

					for (currentButtonIndex; currentButtonIndex < myButtons.size(); ++currentButtonIndex)
					{
						auto& currentButton = myButtons[currentButtonIndex];

						if (ImGui::MenuItem(currentButton->GetName().c_str(), nullptr, &currentButton->myTestBool))
						{
							currentButton->Invoke();

							for (size_t i = 0; i < myButtons.size(); ++i)
							{
								if (i != currentButtonIndex)
								{
									auto& otherButton = myButtons[i];

									otherButton->myTestBool = false;
									otherButton->Invoke();
								}
							}

							break;
						}
					}

					ImGui::EndMenu();
				}

				ImGui::EndMainMenuBar();
			}
		}

	private:
		std::vector<std::unique_ptr<MenuItemPopUpTest>> myButtons;
	};

	class MenuNormalTest final : public MainMenuTabTestBase
	{
	public:
		MenuNormalTest(const char* aName) : MainMenuTabTestBase(aName) {}

		MenuItemButton* AddButton(const char* aButtonName, std::function<void()> aCallback)
		{
			std::unique_ptr<MenuItemButton> button = std::make_unique<MenuItemButton>(aButtonName, aCallback);
			MenuItemButton* buttonPointer = button.get();
			myButtons.push_back(std::move(button));
			return buttonPointer;
		}

		MenuItemButton* AddButton(std::unique_ptr<MenuItemButton> aButton)
		{
			MenuItemButton* buttonPointer = aButton.get();
			myButtons.push_back(std::move(aButton));
			return buttonPointer;
		}

		MenuItemPopUpTest* AddPopUp(std::unique_ptr<MenuItemPopUpTest> aButton)
		{
			MenuItemPopUpTest* buttonPointer = aButton.get();
			myButtons.push_back(std::move(aButton));
			return buttonPointer;
		}

		MenuItemMenu* AddMenu(const char* aButtonName)
		{
			std::unique_ptr<MenuItemMenu> button = std::make_unique<MenuItemMenu>(aButtonName);
			MenuItemMenu* buttonPointer = button.get();
			myButtons.push_back(std::move(button));
			return buttonPointer;
		}

		MenuItemSelectable* AddSelectable(const char* aButtonName, std::function<void(const std::string&)> aCallback)
		{
			std::unique_ptr<MenuItemSelectable> button = std::make_unique<MenuItemSelectable>(aButtonName, std::move(aCallback));
			MenuItemSelectable* buttonPointer = button.get();
			myButtons.push_back(std::move(button));
			return buttonPointer;
		}

		void Render() override final
		{
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu(myName.c_str()))
				{
					for (const auto& button : myButtons)
					{
						button->Render();
					}

					ImGui::EndMenu();
				}

				ImGui::EndMainMenuBar();
			}
		}
	private:
		std::vector<std::unique_ptr<MenuItemTest>> myButtons;
	};

	std::vector<std::unique_ptr<MainMenuTabTestBase>> globalMainMenuTabs;
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
		/*SetUpSceneTab();
		SetupSettingsTab();
		SetUpHelpTab();*/

		//auto postprocess = AddPopUpWindow<PostProcessPopUp>();
		//postprocess->SetActive(true);

		//auto deferred = AddPopUpWindow<DeferredPopUp>();
		//deferred->SetActive(true);

		/*for (const std::shared_ptr<PopUp> popUp : myPopUpWindows)
		{
			popUp->Init();
		}*/

		std::unique_ptr<MenuNormalTest> scene = std::make_unique< MenuNormalTest>("Scene");


		auto saveButton = scene->AddButton("Save", IWasClicked); saveButton;


		auto newMenu = scene->AddMenu("A New Menu");

		std::unique_ptr<MenuItemButton> button = std::make_unique<MenuItemButton>("New Child Button", IWasClicked);
		std::unique_ptr<MenuItemButton> subMenuButton = std::make_unique<MenuItemButton>("New Sub Child Button", IWasClicked);
		std::unique_ptr<MenuItemMenu> subMenu = std::make_unique<MenuItemMenu>("New Child Menu Button");


		auto selectable = scene->AddSelectable("Selectable", TestSeletectable::SelectableClick); selectable;


		std::shared_ptr<AudioSettingsPopUp> audioSettingPopUP = std::make_shared<AudioSettingsPopUp>("Audio");
		std::shared_ptr<CameraSettingsPopUp> cameraSettingPopUp = std::make_shared<CameraSettingsPopUp>("Camera");
		std::shared_ptr<GraphicsSettingsPopUp> graphicsSettingPopUP = std::make_shared<GraphicsSettingsPopUp>("Graphics");

		globalPopUps.push_back(audioSettingPopUP);
		globalPopUps.push_back(cameraSettingPopUp);
		globalPopUps.push_back(graphicsSettingPopUP);

		std::unique_ptr<MenuItemPopUpTest> popUpTest = std::make_unique<MenuItemPopUpTest>("Pop Up!");

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

		std::unique_ptr<MenuItemPopUpTest> windowPopUp1 = std::make_unique<MenuItemPopUpTest>("Window1!!");
		std::unique_ptr<MenuItemPopUpTest> windowPopUp2 = std::make_unique<MenuItemPopUpTest>("Window2!!");
		std::unique_ptr<MenuWindowTest> window = std::make_unique< MenuWindowTest>("Windows");

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

		/*for (const std::shared_ptr<MainMenuItemTab> tab : myMainMenuTabs)
		{
			tab->Render();
		}

		for (const std::shared_ptr<PopUp> popUp : myPopUpWindows)
		{
			if (popUp->IsActive())
			{
				popUp->Render();
			}
		}*/

		//TempPlayMenuBar();



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
