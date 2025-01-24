#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Core/Items/MenuItemMenu.hpp"

namespace Editor
{
	MenuItemMenu::MenuItemMenu(const char* aName) : MenuItemBase(aName)
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
