#pragma once
#include "Editor/Core/Base/Window.hpp"
#include "Editor/Core/PopUp.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"
#include <string>
#include <memory>
#include <vector>

namespace Editor
{
	class MainMenuItem;

	template <typename T>
	concept DerivedFromMainMenuItem = std::is_base_of_v<MainMenuItem, T>&& std::is_class_v<T>;
}

namespace Editor
{
	class MainMenuItem : public Window
	{
	public:
		MainMenuItem(const std::string& aWindowName);

		void Update();
		void Render();

		//NOTE(v11.4.5): Invoked only if there are no ItemChildren and PopUpChildren. TO-DO: Improve this somehow to work as a button instead of menu
		virtual void OnClick() {};
	public:
		bool IsPopUpActive() const;
	public:
		void SetHotKeyShortCutText(const char* aHotKeyShortCut);
	public:
		const char* GetHotKeyShortCutText() const;
	public:
		template<DerivedFromPopUpWindow T>
		std::shared_ptr<T> AddPopUpWindows(std::shared_ptr<T> aPopUpWindow);

		template<DerivedFromMainMenuItem T>
		std::shared_ptr<T> AddChildren(std::shared_ptr<T> aMainMenuItem);
	protected:
		std::vector<std::shared_ptr<PopUp>> myPopUpWindows;
		std::vector<std::shared_ptr<MainMenuItem>> myChildren;
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

	template<DerivedFromMainMenuItem T>
	inline std::shared_ptr<T> MainMenuItem::AddChildren(std::shared_ptr<T> aMainMenuItem)
	{
		for (const auto& existingChildren : myChildren)
		{
			if (existingChildren == aMainMenuItem)
			{
				return aMainMenuItem;
			}
		}

		myChildren.push_back(aMainMenuItem);
		myChildren.back()->SetActive(myChildren.back()->myPopUpIsActive);

		return aMainMenuItem;
	}
}