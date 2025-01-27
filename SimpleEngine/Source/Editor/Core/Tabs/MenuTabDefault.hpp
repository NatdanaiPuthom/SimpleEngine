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
		MenuItemButton* AddButton(const char* aButtonName);
		MenuItemPopUp* AddPopUp(const char* aPopUpName);
		MenuItemMenu* AddMenu(const char* aMenuButtonName);
		MenuItemSelectable* AddSelectable(const char* aSelectableButtonName);
	private:
		std::vector<std::unique_ptr<MenuItemBase>> myButtons;
	};
}
