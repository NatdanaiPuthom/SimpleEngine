#pragma once
#include "Editor/Core/MainMenuTab.hpp"
#include "Editor/Core/PopUp.hpp"
#include <vector>
#include <memory>
#include <string>
#include <type_traits>
#include <typeindex>

namespace Editor
{
	class EditorEngine final
	{
	public:
		static size_t mySelectedEntityID;
	public:
		EditorEngine();
		~EditorEngine();

		void Init();
		void Update();
		void Render();

		template<DerivedFromPopUpWindow T>
		std::shared_ptr<T> AddPopUpWindow()
		{
			const std::string prettyName = SimpleUtilities::ConvertTypeIndexNameToPrettyName(typeid(T).name());
			std::shared_ptr<T> window = std::make_shared<T>(prettyName);
			myPopUpWindows.push_back(window);
			return window;
		}

		template<DerivedFromMainMenuTab T>
		std::shared_ptr<T> AddMenuTab()
		{
			const std::string prettyName = SimpleUtilities::ConvertTypeIndexNameToPrettyName(typeid(T).name());
			std::shared_ptr<T> tab = std::make_shared<T>(prettyName);
			myMainMenuTabs.push_back(tab);
			return tab;
		}

	private:
		std::vector<std::shared_ptr<PopUp>> myPopUpWindows;
		std::vector<std::shared_ptr<MainMenuTab>> myMainMenuTabs;
	};
}