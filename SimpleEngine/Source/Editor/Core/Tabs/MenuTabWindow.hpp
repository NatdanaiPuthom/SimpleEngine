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
		MenuTabWindow(const char* aName);

		void Render() override final;

	public:
		MenuItemPopUp* AddPopUp(std::unique_ptr<MenuItemPopUp> aButton);
	private:
		std::vector<std::unique_ptr<MenuItemPopUp>> myButtons;
	};
}
