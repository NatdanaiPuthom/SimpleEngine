#pragma once
#include "Editor/Core/Base/Window.hpp"
#include "Editor/Core/PopUp.hpp"
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
		virtual void Render() = 0;

		const char* myHotKeyShortCutText = nullptr;
		bool myPopUpIsActive = false;
	};

	class MainMenuItemButton : public MainMenuItemTest
	{
	public:
		MainMenuItemButton(const std::string& aWindowName) : MainMenuItemTest(aWindowName) {}

		virtual void OnInvoke() = 0;

		void Render() override final
		{
			if (ImGui::MenuItem(myImGuiName.c_str(), myHotKeyShortCutText))
			{
				OnInvoke();
			}
		};
	};

	class MainMenuItemParent : public MainMenuItemTest
	{
	public:
		MainMenuItemParent(const std::string& aWindowName) : MainMenuItemTest(aWindowName) {}

		void InternalUpdate() override final
		{
			if (ImGui::BeginMenu(myImGuiName.c_str()))
			{
				for (std::shared_ptr<MainMenuItemTest> child : myChildren)
				{
					child->InternalUpdate();
				}

				ImGui::EndMenu();
			}
		}

		void Render() override final
		{
			if (ImGui::BeginMenu(myImGuiName.c_str()))
			{
				for (std::shared_ptr<MainMenuItemTest> child : myChildren)
				{
					child->Render();
				}

				ImGui::EndMenu();
			}
		};

	public:
		std::vector<std::shared_ptr<MainMenuItemTest>> myChildren;
	};

	class MainMenuItemPopUp : public MainMenuItemTest
	{
	public:
		MainMenuItemPopUp(const std::string& aWindowName) : MainMenuItemTest(aWindowName) {}

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

		std::vector<std::shared_ptr<PopUp>> myPopUpWindows;
	};

	class MainMenuItemList : public MainMenuItemTest
	{
	public:
		MainMenuItemList(const std::string& aWindowName) : MainMenuItemTest(aWindowName) {}

		void Render() override final
		{
			if (ImGui::BeginMenu(myImGuiName.c_str()))
			{
				for (std::shared_ptr<MainMenuItemButton> button : myButtons)
				{
					if (ImGui::Selectable(button->GetWindowName().c_str()))
					{
						button->OnInvoke();
					}
				}

				ImGui::EndMenu();
			}
		}

		std::vector<std::shared_ptr<MainMenuItemButton>> myButtons;
	};

	class TestButton : public MainMenuItemButton
	{
	public:
		TestButton(const std::string& aWindowName) : MainMenuItemButton(aWindowName) {}

		void OnInvoke() override
		{
			std::cout << "hello world" << std::endl;
		}
	};
}