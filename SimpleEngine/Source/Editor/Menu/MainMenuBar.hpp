#pragma once
#include "Editor/Template/ToolInterface.hpp"

namespace Editor
{
	class HierarchyWindow;
	class AssetWindow;
	class SettingsTool;
	class DeferredSceneWindow;
}

namespace Editor
{
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
		std::unique_ptr<Editor::HierarchyWindow> myHierarchyWindow;
		std::unique_ptr<Editor::AssetWindow> myAssetWindow;
		std::unique_ptr<Editor::DeferredSceneWindow> myDeferredSceneWindow;

		bool myEditorWindowActive;
		bool myDeferredWindowActive;
	};
}