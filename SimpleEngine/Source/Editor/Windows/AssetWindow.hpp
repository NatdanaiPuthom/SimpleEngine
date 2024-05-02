#pragma once

namespace Editor
{
	class AssetWindow final : public Editor::ToolInterface
	{
	public:
		AssetWindow();

		void Init() override;
		void Draw() override;
	};
}