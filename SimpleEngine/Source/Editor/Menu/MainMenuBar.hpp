#pragma once
#include "Editor/Template/ToolInterface.hpp"
#include <memory>

namespace Editor
{
	class HierarchyWindow;
	class AssetWindow;
	class SettingsWindow;
	class DeferredSceneWindow;
	class PostProcessingWindow;
	class NodeScriptingWindow;
}

namespace Editor
{

	enum class eWindowType
	{
		Editor,
		Deferred,
		PostProcess,
		NodeScript,
		Count
	};

	class ActiveWindowData
	{
	public:

		ActiveWindowData() = default;
		bool IsWindowActive(eWindowType aWindowType) const;
		void SetActiveWindow(eWindowType aWindowType, bool aIsActive = true);


	private:

		std::array<bool, static_cast<size_t>(eWindowType::Count)> myIsWindowActive;
	};

	class MainMenuBar : public Editor::ToolInterface
	{
	public:
		MainMenuBar();
		~MainMenuBar();

		void Init() override;
		void Update() override;
		void Draw() override;
	private:
		void MenuItemFullScreen();
		void DrawTools();
		void LoadSettingsFromJson();
	private:
		std::unique_ptr<SettingsWindow> mySettingsWindow;
		std::unique_ptr<HierarchyWindow> myHierarchyWindow;
		std::unique_ptr<AssetWindow> myAssetWindow;
		std::unique_ptr<DeferredSceneWindow> myDeferredSceneWindow;
		std::unique_ptr<PostProcessingWindow> myPostProcessWindow;
		std::unique_ptr<NodeScriptingWindow> myNodeScriptingWindow;

		ActiveWindowData myActiveWindowData;
	};
}