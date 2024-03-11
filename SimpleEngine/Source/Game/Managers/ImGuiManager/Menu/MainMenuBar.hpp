#pragma once

namespace Tool
{
	class SettingsTool;
	class CameraTool;

	class MainMenuBar : public Simple::ToolInterface
	{
	public:
		MainMenuBar();
		~MainMenuBar();

		void Init() override;
		void Draw() override;
	private:
		void DrawTools();
	private:
		std::unique_ptr<SettingsTool> mySettingsTool;
		std::unique_ptr<CameraTool> myCameraTool;

		bool mySettingToolActive;
		bool myCameraToolActive;
	};
}