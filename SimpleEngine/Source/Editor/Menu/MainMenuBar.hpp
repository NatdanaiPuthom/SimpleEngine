#pragma once

namespace Editor
{
	class InspectorWindow;
	class HierarchyWindow;
	class AssetWindow;
}

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
		void Update() override;
		void Draw() override;
	private:
		void DrawTools();
		void LoadSettingsFromJson();
	private:
		std::unique_ptr<SettingsTool> mySettingsTool;
		std::unique_ptr<CameraTool> myCameraTool;
		std::unique_ptr<Editor::InspectorWindow> myInspectorWindow;
		std::unique_ptr<Editor::HierarchyWindow> myHierarchyWindow;
		std::unique_ptr<Editor::AssetWindow> myAssetWindow;

		bool myDebugMenuActive;
		bool mySettingToolActive;
	};
}