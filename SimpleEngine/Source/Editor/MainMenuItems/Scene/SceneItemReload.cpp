#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/MainMenuItems/Scene/SceneItemReload.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace Editor
{
	SceneItemReload::SceneItemReload(const std::string& aName) : MainMenuItem(aName)
	{
	}

	void SceneItemReload::OnClick()
	{
		Simpleton::SceneManager& sceneManager = MainSingleton::GetSceneManager();
		sceneManager.ReloadSceneFromFile(sceneManager.GetCurrentSceneInfo()->relativePath);
	}

	void SceneItemReload::ToolTips()
	{
		ImGui::Text("Reset current scene");
	}
}
