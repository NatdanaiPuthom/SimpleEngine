#pragma once

namespace Editor
{
	class HierarchyWindow final : public Simple::ToolInterface
	{
	public:
		HierarchyWindow();

		void Init() override;
		void Draw() override;
	};
}