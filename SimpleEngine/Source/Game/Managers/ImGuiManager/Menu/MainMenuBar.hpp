#pragma once

namespace Tool
{
	class SettingsTool;
	class RenderTool;
	class MeshTool;
	class CameraTool;
	class LightTool;

	class MainMenuBar : public Simple::ToolInterface
	{
	public:
		MainMenuBar();

		void Init() override;
		void Draw() override;
	private:
		void DrawTools();
	private:
		std::unique_ptr<SettingsTool> mySettingsTool;
		std::unique_ptr<RenderTool> myRenderTool;
		std::unique_ptr<MeshTool> myMeshTool;
		std::unique_ptr<CameraTool> myCameraTool;
		std::unique_ptr<LightTool> myLightTool;

		bool myNodeToolActive;
		bool mySettingToolActive;
		bool myRenderToolActive;
		bool myMeshToolActive;
		bool myCameraToolActive;
		bool myLightToolActive;
	};
}