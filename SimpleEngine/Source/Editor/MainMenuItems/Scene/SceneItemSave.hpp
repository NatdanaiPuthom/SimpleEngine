#pragma once
#include "Editor/Core/MainMenuItem.hpp"

namespace Editor
{
	class SceneItemSave final : public MainMenuItem
	{
	public:
		SceneItemSave(const std::string& aName);

		void OnClick() override;
	};
}
