#pragma once
#include "Editor/Core/PopUp.hpp"
#include "Editor/Core/MainMenuItem.hpp"
#include "Editor/Core/MainMenuTab.hpp"
#include <vector>
#include <memory>
#include <string>
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
	private:
		template<DerivedFromPopUpWindow T>
		std::shared_ptr<T> AddPopUpWindow();

		template<DerivedFromMainMenuTab T>
		std::shared_ptr<T> AddMenuTab();

	private:
		void SetUpSceneTab();
		void SetupSettingsTab();
		void SetUpHelpTab();
	private:
		void TempPlayMenuBar();
	private:
		std::vector<std::shared_ptr<PopUp>> myPopUpWindows;
		std::vector<std::shared_ptr<MainMenuItemTab>> myMainMenuTabs;
	};

	template<DerivedFromPopUpWindow T>
	inline std::shared_ptr<T> EditorEngine::AddPopUpWindow()
	{
		const std::string prettyName = SimpleUtilities::ConvertTypeIndexNameToPrettyName(typeid(T).name());
		std::shared_ptr<T> window = std::make_shared<T>(prettyName);
		myPopUpWindows.push_back(window);
		return window;
	}

	template<DerivedFromMainMenuTab T>
	inline std::shared_ptr<T> EditorEngine::AddMenuTab()
	{
		const std::string prettyName = SimpleUtilities::ConvertTypeIndexNameToPrettyName(typeid(T).name());

		std::shared_ptr<T> tab = std::make_shared<T>(prettyName);
		myMainMenuTabs.push_back(tab);

		return tab;
	}
}