#pragma once
#include "Editor/Core/Base/Window.hpp"
#include <string>

namespace Editor
{
	class MainMenuItem : public Window
	{
	public:
		MainMenuItem(const std::string& aWindowName);
		virtual void Render() = 0;
	};
}