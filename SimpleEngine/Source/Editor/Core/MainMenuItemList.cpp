#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Core/MainMenuItemList.hpp"

namespace Editor
{
	MainMenuItemList::MainMenuItemList(const std::string& aWindowName) : MainMenuItem(aWindowName)
	{
	}

	void MainMenuItemList::Render()
	{
		if (ImGui::BeginMenu(myImGuiName.c_str()))
		{
			for (std::shared_ptr<MainMenuItem> button : myButtons)
			{
				button->Render();
			}

			ImGui::EndMenu();
		}
	}
}
