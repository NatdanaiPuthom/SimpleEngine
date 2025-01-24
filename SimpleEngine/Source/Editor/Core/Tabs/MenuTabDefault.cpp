#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Core/Tabs/MenuTabDefault.hpp"
#include "External/imgui.h"

namespace Editor
{
	MenuTabDefault::MenuTabDefault(const char* aName) : MainMenuTabBase(aName)
	{
	}

	void MenuTabDefault::Render()
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

	MenuItemButton* MenuTabDefault::AddButton(const char* aButtonName, std::function<void()> aCallback)
	{
		std::unique_ptr<MenuItemButton> button = std::make_unique<MenuItemButton>(aButtonName, aCallback);
		MenuItemButton* buttonPointer = button.get();
		myButtons.push_back(std::move(button));
		return buttonPointer;
	}

	MenuItemButton* MenuTabDefault::AddButton(std::unique_ptr<MenuItemButton> aButton)
	{
		MenuItemButton* buttonPointer = aButton.get();
		myButtons.push_back(std::move(aButton));
		return buttonPointer;
	}

	MenuItemPopUp* MenuTabDefault::AddPopUp(std::unique_ptr<MenuItemPopUp> aButton)
	{
		MenuItemPopUp* buttonPointer = aButton.get();
		myButtons.push_back(std::move(aButton));
		return buttonPointer;
	}

	MenuItemMenu* MenuTabDefault::AddMenu(const char* aButtonName)
	{
		std::unique_ptr<MenuItemMenu> button = std::make_unique<MenuItemMenu>(aButtonName);
		MenuItemMenu* buttonPointer = button.get();
		myButtons.push_back(std::move(button));
		return buttonPointer;
	}

	MenuItemSelectable* MenuTabDefault::AddSelectable(const char* aButtonName, std::function<void(const std::string&)> aCallback)
	{
		std::unique_ptr<MenuItemSelectable> button = std::make_unique<MenuItemSelectable>(aButtonName, std::move(aCallback));
		MenuItemSelectable* buttonPointer = button.get();
		myButtons.push_back(std::move(button));
		return buttonPointer;
	}
}
