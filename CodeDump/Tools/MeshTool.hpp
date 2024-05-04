#pragma once
#include "Editor/Template/ToolInterface.hpp"

namespace Editor
{
	class MeshTool final : public Editor::ToolInterface
	{
	public:
		MeshTool();

		void Draw() override;
	};
}
