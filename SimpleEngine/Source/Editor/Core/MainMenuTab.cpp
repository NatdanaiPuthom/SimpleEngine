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

	void MainMenuTab::Update()
	{
		for (std::shared_ptr<MainMenuItem> child : myMainMenuItems)
		{
			if (child->IsPopUpActive())
			{
				child->Update();
			}
		}
	}

	void MainMenuTab::Render()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu(myImGuiName.c_str()))
			{
				if (myMainMenuItems.empty())
				{
					OnClick();
				}
				else
				{
					for (std::shared_ptr<MainMenuItem> child : myMainMenuItems)
					{
						child->Render();
					}
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

	void MainMenuTab::OnClick()
	{
		ImGui::MenuItem("Default##MainMenuTab");
	}
}
