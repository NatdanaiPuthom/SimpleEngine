#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Core/MainMenuItem.hpp"
#include "Engine/Global.hpp"
#include <typeindex>

namespace Editor
{
	MainMenuItem::MainMenuItem(const std::string& aWindowName)
		: Window(aWindowName, typeid(MainMenuItem).name())
		, myHotKeyShortCutText("")
		, myTooltipsTimer(0.0f)
		, myPopUpIsActive(false)
		, myShowTooltips(false)
	{
	}

	void MainMenuItem::InternalUpdate()
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

		if (myShowTooltips)
		{
			if (ImGui::IsItemHovered())
			{
				myTooltipsTimer += Global::GetDeltaTime();

				if (myTooltipsTimer > 0.33f)
				{
					if (ImGui::BeginTooltip())
					{
						ToolTips();
						ImGui::EndTooltip();
					}
				}
			}
			else
			{
				myTooltipsTimer = 0.0f;
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

	void MainMenuItem::SetShowToolTips(const bool aShowToolTips)
	{
		myShowTooltips = aShowToolTips;
	}

	const char* MainMenuItem::GetHotKeyShortCutText() const
	{
		return myHotKeyShortCutText;
	}
}
