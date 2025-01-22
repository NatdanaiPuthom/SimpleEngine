#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/MenuItems/Scene/SceneItemCreateNewButton.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace Editor
{
	SceneItemCreateNewButton::SceneItemCreateNewButton(const std::string& aWindowName) : MainMenuItemButton(aWindowName)
	{
	}

	void Editor::SceneItemCreateNewButton::Invoke()
	{
		Simpleton::SceneManager& sceneManager = MainSingleton::GetSceneManager();

		const std::string absolutePath = SimpleUtilities::GetAbsolutePath(SimpleUtilities::AppendCounterIfAlreadyExist(std::string(SIMPLE_DIR_SCENES) + "\\" + std::string(SIMPLE_FILENAME_NEWSCENE)));
		const std::string relativePath = SimpleUtilities::ConvertAbsolutePathToRelativePath(absolutePath);

		sceneManager.CreateNewScene(absolutePath);
		sceneManager.ChangeScene(relativePath);
	}
}
