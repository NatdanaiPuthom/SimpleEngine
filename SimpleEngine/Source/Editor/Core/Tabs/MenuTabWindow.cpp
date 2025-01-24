#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Core/Tabs/MenuTabWindow.hpp"
#include "External/imgui.h"

namespace Editor
{
	MenuTabWindow::MenuTabWindow(const char* aName) : MainMenuTabBase(aName)
	{
	}

	void MenuTabWindow::Render()
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

	MenuItemPopUp* MenuTabWindow::AddPopUp(std::unique_ptr<MenuItemPopUp> aButton)
	{
		MenuItemPopUp* buttonPointer = aButton.get();
		myButtons.push_back(std::move(aButton));
		return buttonPointer;
	}
}
