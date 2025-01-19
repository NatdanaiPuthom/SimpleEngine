#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/Core/MainMenuItem.hpp"
#include <typeindex>

namespace Editor
{
	MainMenuItem::MainMenuItem(const std::string& aWindowName)
		: Window(aWindowName, typeid(MainMenuItem).name())
	{
	}
}
