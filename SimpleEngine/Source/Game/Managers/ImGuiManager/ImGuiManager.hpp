#pragma once

namespace Simple
{
	class ToolInterface;
}

namespace Simple
{
	class ImGuiManager final
	{
	public:
		ImGuiManager();
		~ImGuiManager();

		void Init();
		void Render();

		void AddTool(std::unique_ptr<Simple::ToolInterface> aTool);
	private:
		std::vector<std::unique_ptr<Simple::ToolInterface>> myTools;
	};
}