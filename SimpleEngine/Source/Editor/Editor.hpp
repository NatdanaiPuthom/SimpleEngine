#pragma once
#include <memory>
#include <vector>

namespace Editor
{
	class ToolInterface;
}

namespace Editor
{
	class EditorEngine final
	{
	public:
		static bool myStaticShouldRenderDebugLines;
	public:
		EditorEngine();
		~EditorEngine();

		void Init();
		void Update();
		void Render();

		void AddTool(std::unique_ptr<ToolInterface> aTool);

	private:
		std::vector<std::unique_ptr<ToolInterface>> myTools;
	};
}