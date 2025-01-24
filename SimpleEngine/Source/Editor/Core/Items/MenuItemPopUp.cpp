#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Core/Items/MenuItemPopUP.hpp"

namespace Editor
{
	MenuItemPopUp::MenuItemPopUp(const char* aName, std::function<void()> aCallback) : MenuItemBase(aName)
		, myIsActive(false)
	{
	}

	void MenuItemPopUp::Render()
	{
		if (ImGui::MenuItem(myName.c_str(), nullptr, &myIsActive) && !myCallback.empty() && myCallback.front())
		{
			for (auto& callback : myCallback)
			{
				callback();
			}
		}
	}

	void MenuItemPopUp::SetCallback(std::function<void()> aCallback)
	{
		myCallback.push_back(std::move(aCallback));
	}

	void MenuItemPopUp::SetIsActive(const bool aShouldActive)
	{
		myIsActive = aShouldActive;
	}

	bool& MenuItemPopUp::GetIsActiveRef()
	{
		return myIsActive;
	}

	void MenuItemPopUp::Invoke()
	{
		if (!myCallback.empty() && myCallback.front())
		{
			for (auto& callback : myCallback)
			{
				callback();
			}
		}
	}
}
