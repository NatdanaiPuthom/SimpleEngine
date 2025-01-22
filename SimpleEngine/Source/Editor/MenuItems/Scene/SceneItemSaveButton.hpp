#pragma once
#include "Editor/Core/MainMenuItemButton.hpp"

namespace Editor
{
	class SceneItemSaveButton : public MainMenuItemButton
	{
	public:
		SceneItemSaveButton(const std::string& aWindowName);

		void Invoke() override;
	};
}