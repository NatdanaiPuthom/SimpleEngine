#pragma once
#include "Editor/Core/Base/Window.hpp"
#include <string>
#include <type_traits>

namespace Editor
{
	class MainMenuItem : public Window
	{
	public:
		MainMenuItem(const std::string& aWindowName) : Window(aWindowName, typeid(MainMenuItem).name()) {}

		virtual void InternalUpdate() {};
		virtual void Invoke() {};
		virtual void Render() = 0;

		const char* myHotKeyShortCutText = nullptr;
		bool myPopUpIsActive = false;
	};
}

namespace Editor
{
	template <typename T>
	concept DerivedFromMainMenuItem = std::is_base_of_v<MainMenuItem, T>&& std::is_class_v<T>;
}
