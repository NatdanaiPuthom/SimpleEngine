#pragma once
#include "Editor/Core/MainMenuItemButton.hpp"
#include <iostream>

namespace Editor
{
	class SceneItemSetAsStartButton : public MainMenuItemButton
	{
	public:
		SceneItemSetAsStartButton(const std::string& aWindowName) : MainMenuItemButton(aWindowName) {}

		void Invoke() override
		{
			std::cout << "set as start!!" << std::endl;
		}
	};
}