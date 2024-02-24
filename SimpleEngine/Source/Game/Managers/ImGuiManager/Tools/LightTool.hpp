#pragma once

namespace Tool
{
	class LightTool final : public Simple::ToolInterface
	{
	public:
		LightTool();

		void Draw() override;
	};
}