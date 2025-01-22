#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Core/MainMenuItemButton.hpp"

namespace Editor
{
	MainMenuItemButton::MainMenuItemButton(const std::string& aWindowName) : MainMenuItem(aWindowName)
	{
	}

	void MainMenuItemButton::Render()
	{
		if (ImGui::MenuItem(myImGuiName.c_str(), myHotKeyShortCutText))
		{
			Invoke();
		}
	}
}
