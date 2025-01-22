#pragma once
#include "Editor/Core/MainMenuItemButton.hpp"
#include "Editor/Core/MainMenuItemSelectable.hpp"
#include "Mainsingleton/MainSingleton.hpp"
#include <iostream>

namespace Editor
{
	class TestButton : public MainMenuItemButton
	{
	public:
		TestButton(const std::string& aWindowName) : MainMenuItemButton(aWindowName) {}

		void Invoke() override
		{
			std::cout << "button clicked!" << std::endl;
		}
	};
}