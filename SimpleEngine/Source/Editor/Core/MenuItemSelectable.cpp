#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Core/MenuItemSelectable.hpp"

namespace Editor
{
	MenuItemSelectable::MenuItemSelectable(const char* aName, std::function<void(const std::string&)> aCallback) : MenuItem(aName)
		, myCallback(std::move(aCallback))
	{
	}

	void MenuItemSelectable::Render()
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

	void MenuItemSelectable::SetStrings(const std::vector<std::string>& aNewStrings)
	{
		myStrings = aNewStrings;
	}
}
