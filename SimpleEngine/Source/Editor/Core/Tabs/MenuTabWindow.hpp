#pragma once
#include "Editor/Core/Base/MenuItemTabBase.hpp"
#include "Editor/Core/Items/MenuItemPopUp.hpp"
#include <vector>
#include <memory>

namespace Editor
{
	class MenuTabWindow final : public MainMenuTabBase
	{
	public:
		MenuTabWindow(const char* aName, const char* aImGuiTag);

		void Render() override final;
	
		void ActiveWindow(MenuItemPopUp* aItemPopUp, const bool aShouldActive = true);
	public:
		MenuItemPopUp* AddPopUp(const char* aPopUpButtonName);
	private:
		std::vector<std::unique_ptr<MenuItemPopUp>> myButtons;
	};
}
