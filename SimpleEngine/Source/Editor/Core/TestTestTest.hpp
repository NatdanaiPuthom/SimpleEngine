#pragma once
#include "Editor/Core/Base/Window.hpp"
#include "Editor/Core/PopUp.hpp"
#include "Engine/SimpleUtilities/FileManager/FileManager.hpp"
#include <string>
#include <memory>
#include <vector>

namespace Editor
{
	class MainMenuItemTest : public Window
	{
	public:
		MainMenuItemTest(const std::string& aWindowName) : Window(aWindowName, typeid(MainMenuItemTest).name()) {}

		virtual void InternalUpdate() {};
		virtual void Invoke() {};
		virtual void Render() = 0;

		const char* myHotKeyShortCutText = nullptr;
		bool myPopUpIsActive = false;
	};

	class MainMenuItemButton : public MainMenuItemTest
	{
	public:
		MainMenuItemButton(const std::string& aWindowName) : MainMenuItemTest(aWindowName) {}

		virtual void Invoke() = 0;

		void Render() override final
		{
			if (ImGui::MenuItem(myImGuiName.c_str(), myHotKeyShortCutText))
			{
				Invoke();
			}
		};
	};

	class MainMenuItemParent final : public MainMenuItemTest
	{
	public:
		MainMenuItemParent(const std::string& aWindowName) : MainMenuItemTest(aWindowName) {}

		void InternalUpdate() override final
		{
			for (std::shared_ptr<MainMenuItemTest> child : myChildren)
			{
				child->InternalUpdate();
			}
		}

		void Render() override final
		{
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu(myImGuiName.c_str()))
				{
					for (std::shared_ptr<MainMenuItemTest> child : myChildren)
					{
						child->Render();
					}

					ImGui::EndMenu();
				}

				ImGui::EndMainMenuBar();
			}
		};

		template<typename T> requires std::is_base_of_v<MainMenuItemTest, T>&& std::is_class_v<T>
		std::shared_ptr<T> AddChild(std::shared_ptr<T> aChild)
		{
			for (const auto& existingPopUpWindow : myChildren)
			{
				if (existingPopUpWindow == aChild)
				{
					return aChild;
				}
			}

			myChildren.push_back(aChild);
			return aChild;
		}

	private:
		std::vector<std::shared_ptr<MainMenuItemTest>> myChildren;
	};

	class MainMenuItemPopUp final : public MainMenuItemTest
	{
	public:
		MainMenuItemPopUp(const std::string& aWindowName) : MainMenuItemTest(aWindowName) {}

		void InternalUpdate() override final
		{
			if (!myPopUpWindows.empty())
			{
				myPopUpIsActive = myPopUpWindows.front()->IsActive();

			}
		}

		void Render() override final
		{
			if (ImGui::MenuItem(myImGuiName.c_str(), myHotKeyShortCutText, &myPopUpIsActive))
			{
				for (std::shared_ptr<PopUp> popUpWindow : myPopUpWindows)
				{
					popUpWindow->SetActive(myPopUpIsActive);
				}
			}
		};

		template<typename T> requires  std::is_base_of_v<PopUp, T>&& std::is_class_v<T>
		std::shared_ptr<T> AddPopUpWindows(std::shared_ptr<T> aPopUpWindow)
		{
			for (const auto& existingPopUpWindow : myPopUpWindows)
			{
				if (existingPopUpWindow == aPopUpWindow)
				{
					return aPopUpWindow;
				}
			}

			myPopUpWindows.push_back(aPopUpWindow);
			return aPopUpWindow;
		}

	private:
		std::vector<std::shared_ptr<PopUp>> myPopUpWindows;
	};

	class MainMenuItemList final : public MainMenuItemTest
	{
	public:
		MainMenuItemList(const std::string& aWindowName) : MainMenuItemTest(aWindowName) {}

		void Render() override final
		{
			if (ImGui::BeginMenu(myImGuiName.c_str()))
			{
				for (std::shared_ptr<MainMenuItemTest> button : myButtons)
				{
					button->Render();
				}

				ImGui::EndMenu();
			}
		}

		template<typename T> requires std::derived_from<T, MainMenuItemButton> || std::derived_from<T, MainMenuItemPopUp>
		void AddChild(std::shared_ptr<T> aButton)
		{
			myButtons.push_back(aButton);
		}

	private:
		std::vector<std::shared_ptr<MainMenuItemTest>> myButtons;
	};

	class MainMenuItemSelector : public MainMenuItemTest
	{
	public:
		MainMenuItemSelector(const std::string& aWindowName) : MainMenuItemTest(aWindowName) {}

		virtual void Invoke(const char* aText) = 0;

		void AddString(const std::string& aString)
		{
			myStrings.push_back(aString);
		}

		void ClearStrings()
		{
			myStrings.clear();
		}

		void Render()
		{
			if (ImGui::BeginMenu(myImGuiName.c_str()))
			{
				for (const auto& text : myStrings)
				{
					if (ImGui::Selectable(text.c_str()))
					{
						Invoke(text.c_str());
						break;
					}
				}

				ImGui::EndMenu();
			}
		}

	public:
		std::vector<std::string> myStrings;
	};

	class TestButton : public MainMenuItemButton
	{
	public:
		TestButton(const std::string& aWindowName) : MainMenuItemButton(aWindowName) {}

		void Invoke() override
		{
			std::cout << "button clicked!" << std::endl;
		}
	};

	class TestSelector : public MainMenuItemSelector
	{
	public:
		TestSelector(const std::string& aWindowName) : MainMenuItemSelector(aWindowName) {}

		void Invoke(const char* aText)
		{
			const std::string scenePath = std::string(SIMPLE_DIR_SCENES) + "\\" + aText;
			MainSingleton::GetSceneManager().ChangeScene(scenePath);
			std::cout << "loadddd!" << std::endl;
		}
	};
}