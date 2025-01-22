#pragma once
#include "Editor/Core/PopUp.hpp"
#include "Editor/Core/TestTestTest.hpp"
#include <vector>
#include <memory>
#include <string>
#include <typeindex>

namespace Editor
{
	template <typename T>
	concept DerivedFromMainMenuParent = std::derived_from<MainMenuItemParent, T>;
}

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

		template<DerivedFromMainMenuParent T>
		std::shared_ptr<T> AddMenuParent();
	private:
		void SetUpSceneTab();
		void SetupSettingsTab();
		void SetUpHelpTab();
	private:
		std::vector<std::shared_ptr<PopUp>> myPopUpWindows;
		std::vector<std::shared_ptr<MainMenuItemParent>> myMainMenuTabParents;

	};

	template<DerivedFromPopUpWindow T>
	inline std::shared_ptr<T> EditorEngine::AddPopUpWindow()
	{
		const std::string prettyName = SimpleUtilities::ConvertTypeIndexNameToPrettyName(typeid(T).name());
		std::shared_ptr<T> window = std::make_shared<T>(prettyName);
		myPopUpWindows.push_back(window);
		return window;
	}

	template<DerivedFromMainMenuParent T>
	inline std::shared_ptr<T> EditorEngine::AddMenuParent()
	{
		const std::string prettyName = SimpleUtilities::ConvertTypeIndexNameToPrettyName(typeid(T).name());

		std::shared_ptr<T> tab = std::make_shared<T>(prettyName);
		myMainMenuTabParents.push_back(tab);

		return tab;
	}
}