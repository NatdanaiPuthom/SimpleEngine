#pragma once

namespace Simple
{
	class ToolInterface
	{
	public:
		virtual ~ToolInterface() = default;

		virtual void Init() {};
		virtual void Draw() = 0;
	};
}