#pragma once

namespace Tool
{
	class Camera final : public Simple::ToolInterface
	{
	public:
		Camera();

		virtual void Draw() override;
	};
}