#pragma once

namespace Editor
{
	class HierarchyWindow;
	class AssetWindow;
}

namespace Editor
{
	class SettingsTool;
	class CameraTool;

	class MainMenuBar : public Editor::ToolInterface
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
		std::unique_ptr<Editor::HierarchyWindow> myHierarchyWindow;
		std::unique_ptr<Editor::AssetWindow> myAssetWindow;

		bool myDebugMenuActive;
		bool mySettingToolActive;
	};
}