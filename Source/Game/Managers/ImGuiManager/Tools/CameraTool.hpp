#pragma once

namespace Tool
{
	class CameraTool final : public Simple::ToolInterface
	{
	public:
		CameraTool();

		virtual void Draw() override;
	};
}