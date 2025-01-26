#pragma once
#include "Editor/Core/PopUp.hpp"
#include "Editor/Core/Base/MenuItemTabBase.hpp"
#include "Editor/Command/Core/EditorCommandTracker.hpp"
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

		template<DerivedFromMainMenuTabBase T>
		T* AddMenuTab(const char* aName);

	private:
		void SetUpDefaultLayout();
	private:
		std::vector<std::shared_ptr<PopUp>> myPopUpWindows;
		std::vector<std::unique_ptr<MainMenuTabBase>> myMainMenuTabs;
		CommandTracker myCommandTracker;
	};

	template<DerivedFromPopUpWindow T>
	inline std::shared_ptr<T> EditorEngine::AddPopUpWindow()
	{
		const std::string prettyName = SimpleUtilities::ConvertTypeIndexNameToPrettyName(typeid(T).name());
		std::shared_ptr<T> window = std::make_shared<T>(prettyName);
		myPopUpWindows.push_back(window);
		return window;
	}

	template<DerivedFromMainMenuTabBase T>
	inline T* EditorEngine::AddMenuTab(const char* aName)
	{
		const std::string tag = SimpleUtilities::ConvertTypeIndexNameToPrettyName(typeid(T).name());

		std::unique_ptr<T> tab = std::make_unique<T>(aName, tag.c_str());
		T* pointer = tab.get();

		myMainMenuTabs.push_back(std::move(tab));

		return pointer;
	}
}