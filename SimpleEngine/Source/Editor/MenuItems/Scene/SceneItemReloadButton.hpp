#pragma once
#include "Editor/Core/MainMenuItemButton.hpp"
#include <iostream>

namespace Editor
{
	class SceneItemReloadButton : public MainMenuItemButton
	{
	public:
		SceneItemReloadButton(const std::string& aWindowName) : MainMenuItemButton(aWindowName) {}

		void Invoke() override
		{
			std::cout << "reloaded!" << std::endl;
		}
	};
}