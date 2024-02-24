#pragma once

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