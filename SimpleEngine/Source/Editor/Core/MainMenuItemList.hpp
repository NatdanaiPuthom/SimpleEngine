#pragma once
#include "Editor/Core/MainMenuItem.hpp"
#include "Editor/Core/MainMenuItemButton.hpp"
#include "Editor/Core/MainMenuItemPopUp.hpp"
#include <string>
#include <memory>
#include <vector>

namespace Editor
{
	class MainMenuItemList final : public MainMenuItem
	{
	public:
		MainMenuItemList(const std::string& aWindowName);

		void Render() override final;

	public:
		template<DerivedFromMainMenuItemButton T>
		void AddChild(std::shared_ptr<T> aButton);
	private:
		std::vector<std::shared_ptr<MainMenuItem>> myButtons;
	};

	template<DerivedFromMainMenuItemButton T>
	inline void MainMenuItemList::AddChild(std::shared_ptr<T> aButton)
	{
		myButtons.push_back(aButton);
	}
}