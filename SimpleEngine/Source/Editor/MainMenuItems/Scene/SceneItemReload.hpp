#pragma once
#include "Editor/Core/MainMenuItem.hpp"

namespace Editor
{
	class SceneItemReload final : public MainMenuItem
	{
	public:
		SceneItemReload(const std::string& aName);

		void OnClick() override;
		void ToolTips() override;
	};
}
