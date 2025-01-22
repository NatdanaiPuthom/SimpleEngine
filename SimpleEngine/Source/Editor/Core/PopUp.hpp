#pragma once
#include "Editor/Core/Base/Window.hpp"
#include <type_traits>

namespace Editor
{
	class PopUp : public Window
	{
	public:
		PopUp(const std::string& aWindowName);

		void Update();
		virtual void Init();
		virtual void Render() = 0;
		virtual void LoadSettingsFromJson(const std::string& aAbsoluteFilePath) { aAbsoluteFilePath; };
	};
}

namespace Editor
{
	template <typename T>
	concept DerivedFromPopUpWindow = std::is_base_of_v<PopUp, T>&& std::is_class_v<T>;
}