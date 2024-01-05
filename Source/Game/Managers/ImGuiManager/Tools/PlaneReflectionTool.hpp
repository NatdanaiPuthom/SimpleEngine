#pragma once

namespace Tool
{
	class PlaneReflectionTool : public Simple::ToolInterface
	{
	public:
		PlaneReflectionTool();

		void Draw() override;

	private:
		void WaterReflectionModel();
	};
}