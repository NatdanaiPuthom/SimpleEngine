#pragma once
#include "Editor/Core/MainMenuItem.hpp"
#include <string>
#include <vector>

namespace Editor
{
	class MainMenuItemSelectable : public MainMenuItem
	{
	public:
		MainMenuItemSelectable(const std::string& aWindowName);

		virtual void Invoke(const char* aText) = 0;

		void Render() override;
	public:
		void AddString(const std::string& aString);
		void ClearStrings();
	public:
		std::vector<std::string> myStrings;
	};
}