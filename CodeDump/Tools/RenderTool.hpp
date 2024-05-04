#pragma once
#include "Editor/Template/ToolInterface.hpp"

namespace Editor
{
	class RenderTool final : public Editor::ToolInterface
	{
	public:
		RenderTool();

		void Draw() override;
	private:
		void ImGuiImage();
		void WaterReflectionModel();
	};
}