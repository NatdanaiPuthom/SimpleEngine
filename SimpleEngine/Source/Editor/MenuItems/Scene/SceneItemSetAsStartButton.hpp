#pragma once
#include "Editor/Core/MainMenuItemButton.hpp"

namespace Editor
{
	class SceneItemSetAsStartButton : public MainMenuItemButton
	{
	public:
		SceneItemSetAsStartButton(const std::string& aWindowName);

		void Invoke() override;
	};
}