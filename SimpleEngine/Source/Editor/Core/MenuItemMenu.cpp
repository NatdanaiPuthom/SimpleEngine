#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Core/MenuItemMenu.hpp"

namespace Editor
{
	MenuItemMenu::MenuItemMenu(const char* aName) : MenuItem(aName)
	{
	}

	void MenuItemMenu::Render()
	{
		if (ImGui::BeginMenu(myName.c_str()))
		{
			for (const auto& child : myItems)
			{
				child->Render();
			}

			ImGui::EndMenu();
		}
	}
}
