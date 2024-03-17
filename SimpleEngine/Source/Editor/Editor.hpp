#pragma once

namespace Simple
{
	class ToolInterface;
}

namespace Simple
{
	class Editor final
	{
	public:
		Editor();
		~Editor();

		void Init();
		void Update();
		void Render();

		void AddTool(std::unique_ptr<Simple::ToolInterface> aTool);
	private:
		std::vector<std::unique_ptr<Simple::ToolInterface>> myTools;
	};
}