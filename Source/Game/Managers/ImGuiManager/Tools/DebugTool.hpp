#pragma once

namespace Tool
{
	class DebugTool final : public Simple::ToolInterface
	{
	public:
		DebugTool();
		virtual ~DebugTool() override;

		virtual void Draw() override;
	private:
		void LoadDataFromJson();
		void WaterPlane();
	private:
		std::vector<int> mySceneIndexes;
		int myActiveScene;
	};
}