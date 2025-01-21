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
		if (myChildren.empty())
		{
			if (myPopUpWindows.empty())
			{
				if (ImGui::MenuItem(myImGuiName.c_str(), myHotKeyShortCutText))
				{
					OnClick();
				}
			}
			else
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
		else
		{
			if (ImGui::BeginMenu(myImGuiName.c_str()))
			{
				for (std::shared_ptr<MainMenuItem> child : myChildren)
				{
					if (ImGui::MenuItem(child->myImGuiName.c_str(), child->myHotKeyShortCutText, &child->myPopUpIsActive))
					{
						for (std::shared_ptr<PopUp> popUpWindow : child->myPopUpWindows)
						{
							popUpWindow->SetActive(child->myPopUpIsActive);
						}
					}
				}

				ImGui::EndMenu();
			}
		}
	}

	bool MainMenuItem::IsPopUpActive() const
	{
		return myPopUpIsActive;
	}

	void MainMenuItem::SetHotKeyShortCutText(const char* aHotKeyShortCut)
	{
		myHotKeyShortCutText = aHotKeyShortCut;
	}

	const char* MainMenuItem::GetHotKeyShortCutText() const
	{
		return myHotKeyShortCutText;
	}
}
