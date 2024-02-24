#pragma once

namespace Tool
{
	class MeshTool final : public Simple::ToolInterface
	{
	public:
		MeshTool();

		void Draw() override;
	};
}
