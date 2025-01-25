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
}