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

	class TestSelector : public MainMenuItemSelector
	{
	public:
		TestSelector(const std::string& aWindowName) : MainMenuItemSelector(aWindowName) {}

		void Invoke(const char* aText)
		{
			const std::string scenePath = std::string(SIMPLE_DIR_SCENES) + "\\" + aText;
			MainSingleton::GetSceneManager().ChangeScene(scenePath);
			std::cout << "loadddd!" << std::endl;
		}
	};
}