#pragma once
#include "Editor/Template/ToolInterface.hpp"

namespace Editor
{
	class LightTool final : public Editor::ToolInterface
	{
	public:
		LightTool();

		void Draw() override;
	};
}