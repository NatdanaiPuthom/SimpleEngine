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

		void AddTool(std::unique_ptr<Simple::ToolInterface> aTool);
		void Render();
	private:
		std::vector<std::unique_ptr<Simple::ToolInterface>> myTools;
	};
}