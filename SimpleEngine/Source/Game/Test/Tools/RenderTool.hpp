#pragma once
#include "Editor/Template/ToolInterface.hpp"

namespace Tool
{
	class RenderTool final : public Simple::ToolInterface
	{
	public:
		RenderTool();

		void Draw() override;
	private:
		void ImGuiImage();
		void WaterReflectionModel();
	};
}