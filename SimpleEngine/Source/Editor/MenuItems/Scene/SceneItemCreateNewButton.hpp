#pragma once
#include "Editor/Core/MainMenuItemButton.hpp"

namespace Editor
{
	class SceneItemCreateNewButton final : public MainMenuItemButton
	{
	public:
		SceneItemCreateNewButton(const std::string& aWindowName);

		void Invoke() override final;
	};
}