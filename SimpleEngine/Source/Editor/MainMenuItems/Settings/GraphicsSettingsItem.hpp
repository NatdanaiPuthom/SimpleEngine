#pragma once
#include "Editor/Core/MainMenuItem.hpp"

namespace Editor
{
	class GraphicsSettingsItem final : public MainMenuItem
	{
	public:
		GraphicsSettingsItem(const std::string& aName);
	};
}
