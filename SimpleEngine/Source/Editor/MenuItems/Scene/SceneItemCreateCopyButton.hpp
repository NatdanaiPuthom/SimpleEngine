#pragma once
#include "Editor/Core/MainMenuItemButton.hpp"
#include <iostream>

namespace Editor
{
	class SceneItemCreateNewCopyButton : public MainMenuItemButton
	{
	public:
		SceneItemCreateNewCopyButton(const std::string& aWindowName) : MainMenuItemButton(aWindowName) {}

		void Invoke() override
		{
			std::cout << "create new copy!" << std::endl;
		}
	};
}