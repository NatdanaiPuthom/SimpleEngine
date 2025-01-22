#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Core/MainMenuItemSelectable.hpp"

namespace Editor
{
	MainMenuItemSelector::MainMenuItemSelector(const std::string& aWindowName): MainMenuItem(aWindowName)
	{
	}

	void MainMenuItemSelector::AddString(const std::string& aString)
	{
		myStrings.push_back(aString);
	}

	void MainMenuItemSelector::ClearStrings()
	{
		myStrings.clear();
	}

	void MainMenuItemSelector::Render()
	{
		if (ImGui::BeginMenu(myImGuiName.c_str()))
		{
			for (const auto& text : myStrings)
			{
				if (ImGui::Selectable(text.c_str()))
				{
					Invoke(text.c_str());
					break;
				}
			}

			ImGui::EndMenu();
		}
	}
}
