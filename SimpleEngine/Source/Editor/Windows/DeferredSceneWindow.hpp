#pragma once
#include "Editor/Template/ToolInterface.hpp"

namespace Editor
{
	class DeferredSceneWindow final : public Editor::ToolInterface
	{
	public:
		DeferredSceneWindow();

		void Init() override;
		void Update() override;
		void Draw() override;
	};
}