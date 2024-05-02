#pragma once

namespace Editor
{
	class AssetWindow final : public Simple::ToolInterface
	{
	public:
		AssetWindow();

		void Init() override;
		void Draw() override;
	};
}