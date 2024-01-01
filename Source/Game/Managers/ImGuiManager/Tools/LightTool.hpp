#pragma once

namespace Tool
{
	class Light final : public Simple::ToolInterface
	{
	public:
		Light();

		virtual void Draw() override;
	};
}