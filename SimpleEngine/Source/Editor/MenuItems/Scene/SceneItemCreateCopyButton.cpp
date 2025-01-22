#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/MenuItems/Scene/SceneItemCreateCopyButton.hpp"
#include "Engine/SimpleUtilities/Utility.hpp"
#include "Engine/Debugger/Console/Console.hpp"
#include "MainSingleton/MainSingleton.hpp"
#include <filesystem>

namespace Editor
{
	SceneItemCreateNewCopyButton::SceneItemCreateNewCopyButton(const std::string& aWindowName) : MainMenuItemButton(aWindowName)
	{
	}

	void SceneItemCreateNewCopyButton::Invoke()
	{
		Simpleton::SceneManager& sceneManager = MainSingleton::GetSceneManager();

		const std::string absolutePath = sceneManager.GetCurrentSceneInfo()->absolutePath;
		const std::string newCopyName = SimpleUtilities::AppendStringBeforeDot("_Copy", absolutePath);
		const std::string newFileName = SimpleUtilities::AppendCounterIfAlreadyExist(newCopyName);
		const std::string relativePath = SimpleUtilities::ConvertAbsolutePathToRelativePath(newFileName);

		std::filesystem::copy_file(absolutePath, newFileName, std::filesystem::copy_options::overwrite_existing);
		sceneManager.ChangeScene(relativePath);

		Simple::Console::Print("New scene: ", Simple::ConsoleTextColor::White, false);
		Simple::Console::Print(sceneManager.GetCurrentSceneInfo()->name.c_str(), Simple::ConsoleTextColor::Green, false);
		Simple::Console::Print(" has been created!", Simple::ConsoleTextColor::White, true);
	}
}
