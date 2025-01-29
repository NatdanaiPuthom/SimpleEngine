#pragma once
#include "Editor/Core/PopUp.hpp"

namespace Editor
{
	class SceneInspectorPopUp final : public PopUp
	{
	public:
		SceneInspectorPopUp(const std::string& aName);

		void Render() override;
	};
}
