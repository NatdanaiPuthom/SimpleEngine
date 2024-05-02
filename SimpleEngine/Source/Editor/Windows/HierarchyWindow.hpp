#pragma once

namespace Editor
{
	class HierarchyWindow final : public Editor::ToolInterface
	{
	public:
		HierarchyWindow();

		void Init() override;
		void Update() override;
		void Draw() override;
	};
}