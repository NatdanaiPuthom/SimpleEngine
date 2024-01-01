#pragma once

namespace Tool
{
	class Mesh final : public Simple::ToolInterface
	{
	public:
		Mesh();

		virtual void Draw() override;
	};
}
