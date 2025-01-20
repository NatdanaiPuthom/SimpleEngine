#pragma once
#include "Editor/Core/Base/Window.hpp"

namespace Editor
{
	class PopUp : public Window
	{
	public:
		PopUp(const std::string& aWindowName);

		void Update();
		virtual void Render() = 0;
	};
}

namespace Editor
{
	template <typename T>
	concept DerivedFromPopUpWindow = std::is_base_of_v<PopUp, T>&& std::is_class_v<T>;
}