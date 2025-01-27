#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Core/Tabs/MenuTabDefault.hpp"
#include "External/imgui.h"

namespace Editor
{
	MenuTabDefault::MenuTabDefault(const char* aName, const char* aImGuiTag) 
		: MainMenuTabBase(aName, aImGuiTag)
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

	MenuItemButton* MenuTabDefault::AddButton(const char* aButtonName)
	{
		std::unique_ptr<MenuItemButton> button = std::make_unique<MenuItemButton>(aButtonName);

		MenuItemButton* buttonPointer = button.get();
		buttonPointer->SetImGuiTag(std::string("##" + myName).c_str());

		myButtons.push_back(std::move(button));
		return buttonPointer;
	}

	MenuItemPopUp* MenuTabDefault::AddPopUp(const char* aPopUpName)
	{
		std::unique_ptr<MenuItemPopUp> button = std::make_unique<MenuItemPopUp>(aPopUpName);

		MenuItemPopUp* buttonPointer = button.get();;
		buttonPointer->SetImGuiTag(std::string("##" + myName).c_str());

		myButtons.push_back(std::move(button));
		return buttonPointer;
	}

	MenuItemMenu* MenuTabDefault::AddMenu(const char* aMenuButtonName)
	{
		std::unique_ptr<MenuItemMenu> button = std::make_unique<MenuItemMenu>(aMenuButtonName);

		MenuItemMenu* buttonPointer = button.get();
		buttonPointer->SetImGuiTag(std::string("##" + myName).c_str());

		myButtons.push_back(std::move(button));
		return buttonPointer;
	}

	MenuItemSelectable* MenuTabDefault::AddSelectable(const char* aSelectableButtonName)
	{
		std::unique_ptr<MenuItemSelectable> button = std::make_unique<MenuItemSelectable>(aSelectableButtonName);

		MenuItemSelectable* buttonPointer = button.get();
		buttonPointer->SetImGuiTag(std::string("##" + myName).c_str());

		myButtons.push_back(std::move(button));
		return buttonPointer;
	}
}
