#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Core/MainMenuItemPopUp.hpp"

namespace Editor
{
	MainMenuItemPopUp::MainMenuItemPopUp(const std::string& aWindowName) : MainMenuItem(aWindowName)
	{
	}

	void MainMenuItemPopUp::InternalUpdate()
	{
		if (!myPopUpWindows.empty())
		{
			myPopUpIsActive = myPopUpWindows.front()->IsActive();

		}
	}

	void MainMenuItemPopUp::Render()
	{
		if (ImGui::MenuItem(myImGuiName.c_str(), myHotKeyShortCutText, &myPopUpIsActive))
		{
			for (std::shared_ptr<PopUp> popUpWindow : myPopUpWindows)
			{
				popUpWindow->SetActive(myPopUpIsActive);
			}
		}
	}
}
