#pragma once
#include "Editor/Core/MainMenuItem.hpp"

namespace Editor
{
	class SceneItemCreate final : public MainMenuItem
	{
	public:
		SceneItemCreate(const std::string& aName);

		void OnClick() override;
	};
}
