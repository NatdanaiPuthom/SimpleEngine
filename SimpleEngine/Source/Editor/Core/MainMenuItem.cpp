#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Core/MainMenuItem.hpp"
#include <typeindex>

namespace Editor
{
	MainMenuItem::MainMenuItem(const std::string& aWindowName)
		: Window(aWindowName, typeid(MainMenuItem).name())
		, myHotKeyShortCutText("")
		, myPopUpIsActive(false)
	{
	}

	void MainMenuItem::Update()
	{
		for (std::shared_ptr<PopUp> popUpWindow : myPopUpWindows)
		{
			myPopUpIsActive = popUpWindow->IsActive();
		}
	}

	void MainMenuItem::Render()
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
