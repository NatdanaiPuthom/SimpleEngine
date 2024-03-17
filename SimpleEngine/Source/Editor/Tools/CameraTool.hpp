#pragma once

namespace Tool
{
	class CameraTool final : public Simple::ToolInterface
	{
	public:
		CameraTool();

		void Draw() override;
	};
}