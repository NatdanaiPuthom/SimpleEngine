#pragma once
#include "Editor/Core/Base/MenuItemTabBase.hpp"
#include "Editor/Core/Items/MenuItemButton.hpp"
#include "Editor/Core/Items/MenuItemPopUp.hpp"
#include "Editor/Core/Items/MenuItemMenu.hpp"
#include "Editor/Core/Items/MenuItemSelectable.hpp"
#include <memory>
#include <functional>
#include <vector>

namespace Editor
{
	class MenuTabDefault final : public MainMenuTabBase
	{
	public:
		MenuTabDefault(const char* aName, const char* aImGuiTag);

		void Render() override final;

	public:
		MenuItemButton* AddButton(const char* aButtonName, std::function<void()> aCallback);
		MenuItemButton* AddButton(std::unique_ptr<MenuItemButton> aButton);

		MenuItemPopUp* AddPopUp(std::unique_ptr<MenuItemPopUp> aButton);

		MenuItemMenu* AddMenu(const char* aButtonName);
		MenuItemMenu* AddMenu(std::unique_ptr<MenuItemMenu> aMenuButton);

		MenuItemSelectable* AddSelectable(const char* aButtonName, std::function<void(const std::string&)> aCallback);
		MenuItemSelectable* AddSelectable(std::unique_ptr<MenuItemSelectable> aSelectableButton);
	private:
		std::vector<std::unique_ptr<MenuItemBase>> myButtons;
	};
}
