#pragma once

namespace Editor
{
	class CameraTool final : public Editor::ToolInterface
	{
	public:
		CameraTool();

		void Draw() override;
	};
}