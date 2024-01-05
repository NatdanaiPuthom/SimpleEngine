#pragma once

namespace Tool
{
	class SceneTool final : public Simple::ToolInterface
	{
	public:
		SceneTool();

		virtual void Draw() override;
	};
}