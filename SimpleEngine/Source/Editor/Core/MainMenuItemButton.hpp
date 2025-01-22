#pragma once
#include "Editor/Core/MainMenuItem.hpp"
#include <string>
#include <type_traits>

namespace Editor
{
	class MainMenuItemButton : public MainMenuItem
	{
	public:
		MainMenuItemButton(const std::string& aWindowName);

		virtual void Invoke() = 0;

		void Render() override final;
	};
}

namespace Editor
{
	template<typename T>
	concept DerivedFromMainMenuItemButton = std::is_base_of_v<MainMenuItemButton, T>&& std::is_class_v<T>;
}