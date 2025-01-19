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
	template <typename T>
	concept DerivedFromMainMenuItem = std::is_base_of_v<MainMenuItem, T>&& std::is_class_v<T>;

	class MainMenuTab : public Window
	{
	public:
		MainMenuTab(const std::string& aWindowName);

		void Render();

		template<DerivedFromMainMenuItem T>
		void AddChildren()
		{
			const std::string prettyName = SimpleUtilities::ConvertTypeIndexNameToPrettyName(typeid(T).name());
			myMainMenuChildrenTabs.push_back(std::make_shared<T>(prettyName));
		}

	protected:
		std::vector<std::shared_ptr<MainMenuItem>> myMainMenuChildrenTabs;
	};
}
