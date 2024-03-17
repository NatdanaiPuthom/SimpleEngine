#pragma once
#include "Editor/Template/ToolInterface.hpp"

namespace Tool
{
	class MeshTool final : public Simple::ToolInterface
	{
	public:
		MeshTool();

		void Draw() override;
	};
}
