#pragma once
#include "Editor/Core/MainMenuItemButton.hpp"
#include <iostream>

namespace Editor
{
	class SceneItemSaveButton : public MainMenuItemButton
	{
	public:
		SceneItemSaveButton(const std::string& aWindowName) : MainMenuItemButton(aWindowName) {}

		void Invoke() override
		{
			std::cout << "saved!!" << std::endl;
		}
	};
}