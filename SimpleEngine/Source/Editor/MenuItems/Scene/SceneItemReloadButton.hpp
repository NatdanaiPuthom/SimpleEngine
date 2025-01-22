#pragma once
#include "Editor/Core/MainMenuItemButton.hpp"

namespace Editor
{
	class SceneItemReloadButton : public MainMenuItemButton
	{
	public:
		SceneItemReloadButton(const std::string& aWindowName);

		void Invoke() override;
	};
}