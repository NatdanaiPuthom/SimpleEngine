#pragma once
#include "Editor/Core/MenuItem.hpp"
#include <functional>
#include <string>
#include <vector>

namespace Editor
{
	class MenuItemSelectable final : public MenuItem
	{
	public:
		MenuItemSelectable(const char* aName, std::function<void(const std::string&)> aCallback);

		void Render() override final;

	public:
		void SetStrings(const std::vector<std::string>& aNewStrings);
	private:
		std::function<void(const std::string&)> myCallback;
		std::vector<std::string> myStrings;
	};
}
