#pragma once

namespace Tool
{
	class Scene final : public Simple::ToolInterface
	{
	public:
		Scene();

		virtual void Draw() override;
	};
}