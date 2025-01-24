#pragma once
#include "Editor/Core/MenuItemPopUp.hpp"
#include "Editor/Core/MenuItemButton.hpp"
#include "Editor/Core/MenuItemMenu.hpp"
#include "Editor/Core/MenuItemSelectable.hpp"
#include "External/imgui.h"

namespace Editor
{
	class MainMenuTabBase
	{
	public:
		MainMenuTabBase(const char* aName) : myName(aName) {}

		virtual ~MainMenuTabBase() = default;
		virtual void Init() {};
		virtual void Render() = 0;

	protected:
		std::string myName;
	};

	class MenuTabWindow final : public MainMenuTabBase
	{
	public:
		MenuTabWindow(const char* aName) : MainMenuTabBase(aName) {}

		MenuItemPopUp* AddPopUp(std::unique_ptr<MenuItemPopUp> aButton)
		{
			MenuItemPopUp* buttonPointer = aButton.get();
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
		std::vector<std::unique_ptr<MenuItemPopUp>> myButtons;
	};

	class MenuTabNormal final : public MainMenuTabBase
	{
	public:
		MenuTabNormal(const char* aName) : MainMenuTabBase(aName) {}

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

		MenuItemPopUp* AddPopUp(std::unique_ptr<MenuItemPopUp> aButton)
		{
			MenuItemPopUp* buttonPointer = aButton.get();
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
		std::vector<std::unique_ptr<MenuItem>> myButtons;
	};
}
