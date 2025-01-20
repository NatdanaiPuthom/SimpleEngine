#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Core/MainMenuTab.hpp"
#include "External/imgui.h"

namespace Editor
{
	MainMenuTab::MainMenuTab(const std::string& aWindowName)
		: Window(SimpleUtilities::ConvertTypeIndexNameToPrettyName(aWindowName)
			, typeid(MainMenuTab).name())
	{
	}

	void MainMenuTab::Render()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu(myImGuiName.c_str()))
			{
				for (std::shared_ptr<MainMenuItem> child : myMainMenuChildrenTabs)
				{
					child->Render();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
}
