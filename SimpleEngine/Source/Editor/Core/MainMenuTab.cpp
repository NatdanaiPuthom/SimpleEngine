#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Core/MainMenuTab.hpp"

namespace Editor
{
	MainMenuItemTab::MainMenuItemTab(const std::string& aWindowName) : MainMenuItem(aWindowName)
	{
	}

	void MainMenuItemTab::InternalUpdate()
	{
		for (std::shared_ptr<MainMenuItem> child : myChildren)
		{
			child->InternalUpdate();
		}
	}

	void MainMenuItemTab::Render()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu(myImGuiName.c_str()))
			{
				for (std::shared_ptr<MainMenuItem> child : myChildren)
				{
					child->Render();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
}
