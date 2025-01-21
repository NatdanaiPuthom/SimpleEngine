#pragma once
#include "Editor/Core/MainMenuItem.hpp"

namespace Editor
{
	class SceneItemSetAsStart final : public MainMenuItem
	{
	public:
		SceneItemSetAsStart(const std::string& aName);

		void OnClick() override;
		void ToolTips() override;
	};
}
