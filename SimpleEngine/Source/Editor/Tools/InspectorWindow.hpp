#pragma once

namespace Editor
{
	class InspectorWindow final : public Simple::ToolInterface
	{
	public:
		InspectorWindow();

		void Init() override;
		void Draw() override;
	};
}