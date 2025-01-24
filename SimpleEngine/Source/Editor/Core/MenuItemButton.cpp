#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Core/MenuItemButton.hpp"

namespace Editor
{
	MenuItemButton::MenuItemButton(const char* aName, std::function<void()> aCallback) : MenuItem(aName)
		, myCallback(std::move(aCallback))
	{
	}

	void MenuItemButton::Render()
	{
		if (ImGui::MenuItem(myName.c_str()) && myCallback)
		{
			myCallback();
		}
	}

	void MenuItemButton::SetCallback(std::function<void()> aCallback)
	{
		myCallback = aCallback;
	}
}
