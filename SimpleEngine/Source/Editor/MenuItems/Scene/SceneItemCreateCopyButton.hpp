#pragma once
#include "Editor/Core/MainMenuItemButton.hpp"

namespace Editor
{
	class SceneItemCreateNewCopyButton final : public MainMenuItemButton
	{
	public:
		SceneItemCreateNewCopyButton(const std::string& aWindowName);

		void Invoke() override final;
	};
}