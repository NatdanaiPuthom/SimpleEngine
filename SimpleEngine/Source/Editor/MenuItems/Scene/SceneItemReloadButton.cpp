#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/MenuItems/Scene/SceneItemReloadButton.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace Editor
{
	SceneItemReloadButton::SceneItemReloadButton(const std::string& aWindowName) : MainMenuItemButton(aWindowName)
	{
	}

	void SceneItemReloadButton::Invoke()
	{
		Simpleton::SceneManager& sceneManager = MainSingleton::GetSceneManager();
		sceneManager.ReloadSceneFromFile(sceneManager.GetCurrentSceneInfo()->relativePath);
	}
}
