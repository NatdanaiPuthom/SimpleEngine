#pragma once
#include "Editor/Core/MainMenuTab.hpp"

namespace Editor
{
	class SceneTab final : public MainMenuTab
	{
	public:
		SceneTab(const std::string& aName);

		void OnClick();
	private:
		void SetActiveSceneAsStart();
	private:
		void CreateNewScene();
	private:
		void SaveActiveScene();
		void ReloadScene();
		void ShowSceneList();
		void ShowHoveredToolTips(const char* aToolTipText, float& aTimer);
	};
}
