#pragma once
#include "Editor/Core/MainMenuItem.hpp"

namespace Editor
{
	class SceneItemLoad final : public MainMenuItem
	{
	public:
		SceneItemLoad(const std::string& aName);

		void OnClick() override;
	};
}
