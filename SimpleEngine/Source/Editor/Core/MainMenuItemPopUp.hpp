#pragma once
#include "Editor/Core/MainMenuItem.hpp"
#include "Editor/Core/PopUp.hpp"
#include <string>
#include <memory>
#include <vector>

namespace Editor
{ 
	class MainMenuItemPopUp final : public MainMenuItem
	{
	public:
		MainMenuItemPopUp(const std::string& aWindowName);

		void InternalUpdate() override final;
		void Render() override final;

	public:
		template<DerivedFromPopUpWindow T>
		std::shared_ptr<T> AddPopUpWindows(std::shared_ptr<T> aPopUpWindow);
	private:
		std::vector<std::shared_ptr<PopUp>> myPopUpWindows;
	};

	template<DerivedFromPopUpWindow T>
	inline std::shared_ptr<T> MainMenuItemPopUp::AddPopUpWindows(std::shared_ptr<T> aPopUpWindow)
	{
		for (const auto& existingPopUpWindow : myPopUpWindows)
		{
			if (existingPopUpWindow == aPopUpWindow)
			{
				return aPopUpWindow;
			}
		}

		myPopUpWindows.push_back(aPopUpWindow);

		return aPopUpWindow;
	}
}