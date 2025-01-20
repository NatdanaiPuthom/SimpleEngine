#pragma once
#include "Editor/Core/MainMenuItem.hpp"

namespace Editor
{
	class CameraSettingsItem final : public MainMenuItem
	{
	public:
		CameraSettingsItem(const std::string& aName);
	};
}
