#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Core/MainMenuItemSelectable.hpp"

namespace Editor
{
	MainMenuItemSelectable::MainMenuItemSelectable(const std::string& aWindowName): MainMenuItem(aWindowName)
	{
	}

	void MainMenuItemSelectable::AddString(const std::string& aString)
	{
		myStrings.push_back(aString);
	}

	void MainMenuItemSelectable::ClearStrings()
	{
		myStrings.clear();
	}

	void MainMenuItemSelectable::Render()
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
