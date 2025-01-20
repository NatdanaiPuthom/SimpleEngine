#pragma once
#include "Editor/Core/MainMenuItem.hpp"

namespace Editor
{
	class AudioItem final : public MainMenuItem
	{
	public:
		AudioItem(const std::string& aName);
	};
}
