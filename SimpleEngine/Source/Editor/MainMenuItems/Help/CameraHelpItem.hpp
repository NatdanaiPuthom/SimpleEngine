#pragma once
#include "Editor/Core/MainMenuItem.hpp"

namespace Editor
{
	class CameraHelpItem final : public MainMenuItem
	{
	public:
		CameraHelpItem(const std::string& aWindowName);
	};
}