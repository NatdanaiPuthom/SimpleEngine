#pragma once
#include "Editor/Template/ToolInterface.hpp"

namespace Tool
{
	class LightTool final : public Simple::ToolInterface
	{
	public:
		LightTool();

		void Draw() override;
	};
}