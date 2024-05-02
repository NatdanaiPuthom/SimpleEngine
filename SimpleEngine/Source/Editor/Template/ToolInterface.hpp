#pragma once

namespace Editor
{
	class ToolInterface
	{
	public:
		virtual ~ToolInterface() = default;

		virtual void Init() {};
		virtual void Update() {};
		virtual void Draw() = 0;
	};
}