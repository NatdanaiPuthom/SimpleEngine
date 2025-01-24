#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Core/MenuItemPopUP.hpp"

namespace Editor
{
	MenuItemPopUp::MenuItemPopUp(const char* aName, std::function<void()> aCallback) : MenuItem(aName)
	{
	}

	void MenuItemPopUp::Render()
	{
		if (ImGui::MenuItem(myName.c_str(), nullptr, &myTestBool) && !myCallback.empty() && myCallback.front())
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
