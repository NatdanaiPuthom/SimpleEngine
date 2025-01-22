#pragma once
#include "Editor/Core/MainMenuItem.hpp"
#include <string>
#include <memory>
#include <vector>
#include <type_traits>

namespace Editor
{
	class MainMenuItemTab final : public MainMenuItem
	{
	public:
		MainMenuItemTab(const std::string& aWindowName);

		void InternalUpdate() override final;
		void Render() override final;

	public:
		template<DerivedFromMainMenuItem T>
		std::shared_ptr<T> AddChild(std::shared_ptr<T> aChild);
	private:
		std::vector<std::shared_ptr<MainMenuItem>> myChildren;
	};

	template<DerivedFromMainMenuItem T>
	inline std::shared_ptr<T> MainMenuItemTab::AddChild(std::shared_ptr<T> aChild)
	{
		for (const auto& existingPopUpWindow : myChildren)
		{
			if (existingPopUpWindow == aChild)
			{
				return aChild;
			}
		}

		myChildren.push_back(aChild);

		return aChild;
	}
}

namespace Editor
{
	template <typename T>
	concept DerivedFromMainMenuTab = std::is_base_of_v<MainMenuItemTab, T>&& std::is_class_v<T>;
}