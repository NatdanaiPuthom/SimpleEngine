#pragma once

namespace Tool
{
	class MeshTool final : public Simple::ToolInterface
	{
	public:
		MeshTool();

		virtual void Draw() override;
	};
}
