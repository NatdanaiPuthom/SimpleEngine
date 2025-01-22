#pragma once
#include "Editor/Core/MainMenuItemButton.hpp"
#include <iostream>

namespace Editor
{
	class SceneItemCreateNewButton : public MainMenuItemButton
	{
	public:
		SceneItemCreateNewButton(const std::string& aWindowName) : MainMenuItemButton(aWindowName) {}

		void Invoke() override
		{
			std::cout << "create new!" << std::endl;
		}
	};
}