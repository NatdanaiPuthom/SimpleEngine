#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/MenuItems/Scene/SceneItemLoadSelectable.hpp"
#include "Mainsingleton/MainSingleton.hpp"
#include "Engine/Debugger/Console/Console.hpp"

namespace Editor
{
	SceneItemLoadSelectable::SceneItemLoadSelectable(const std::string& aWindowName) : MainMenuItemSelectable(aWindowName)
	{ 
	}

	void SceneItemLoadSelectable::Invoke(const char* aText)
	{
		const std::string scenePath = std::string(SIMPLE_DIR_SCENES) + "\\" + aText;
		MainSingleton::GetSceneManager().ChangeScene(scenePath);
		Simple::Console::Print("Loaded scene ", Simple::ConsoleTextColor::White, false);
		Simple::Console::Print(aText, Simple::ConsoleTextColor::Green, false);
		Simple::Console::Print("!", Simple::ConsoleTextColor::White, true);
	}
}
