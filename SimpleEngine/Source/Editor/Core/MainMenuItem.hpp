#pragma once
#include "Editor/Core/Base/Window.hpp"
#include "Editor/Core/PopUp.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"
#include <string>
#include <memory>
#include <vector>

namespace Editor
{
	class MainMenuItem : public Window
	{
	public:
		MainMenuItem(const std::string& aWindowName);

		void Render();
	public:
		template<DerivedFromPopUpWindow T>
		std::shared_ptr<T> AddPopUpWindows(std::shared_ptr<T> aPopUpWindow);
	protected:
		std::vector<std::shared_ptr<PopUp>> myPopUpWindows;
		const char* myHotKeyShortCutText;
		bool myPopUpIsActive;
	};

	template<DerivedFromPopUpWindow T>
	inline std::shared_ptr<T> MainMenuItem::AddPopUpWindows(std::shared_ptr<T> aPopUpWindow)
	{
		for (const auto& existingPopUpWindow : myPopUpWindows)
		{
			if (existingPopUpWindow == aPopUpWindow)
			{
				return aPopUpWindow;
			}
		}

		myPopUpWindows.push_back(aPopUpWindow);
		myPopUpWindows.back()->SetActive(myPopUpIsActive);

		return aPopUpWindow;
	}
}