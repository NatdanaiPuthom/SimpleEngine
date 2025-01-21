#pragma once
#include "Editor/Core/Base/Window.hpp"
#include "Editor/Core/MainMenuItem.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"
#include <vector>
#include <string>
#include <memory>
#include <typeindex>

namespace Editor
{
	class MainMenuTab : public Window
	{
	public:
		MainMenuTab(const std::string& aWindowName);

		void Update();
		void Render();

		template<DerivedFromMainMenuItem T>
		std::shared_ptr<T> AddChildren()
		{
			const std::string prettyName = SimpleUtilities::ConvertTypeIndexNameToPrettyName(typeid(T).name());

			std::shared_ptr<T> menuItem = std::make_shared<T>(prettyName);
			myMainMenuItems.push_back(menuItem);

			return menuItem;
		}

	protected:
		std::vector<std::shared_ptr<MainMenuItem>> myMainMenuItems;
	};
}

namespace Editor
{
	template <typename T>
	concept DerivedFromMainMenuTab = std::is_base_of_v<MainMenuTab, T>&& std::is_class_v<T>;
}