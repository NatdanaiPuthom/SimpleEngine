#pragma once
#include "Editor/Template/ToolInterface.hpp"

namespace Editor
{
	class SceneMenuBar : public Editor::ToolInterface
	{
	public:
		SceneMenuBar();
		~SceneMenuBar();

		void Init() override;
		void Update() override;
		void Draw() override;
	private:
		void SaveActiveScene();
		void ShowSceneList();
		void CreateNewScene();
		void ReloadScene();
		void SetActiveSceneAsStart();
		void ShowHoveredToolTips(const std::string& aToolTipText, float& aTimer);
	};
}