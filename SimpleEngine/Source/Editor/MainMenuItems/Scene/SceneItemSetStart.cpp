#include "Editor/Precomplied/EditorPch.hpp"
#include "Editor/MainMenuItems/Scene/SceneItemSetStart.hpp"
#include "Engine/SimpleUtilities/FileManager/FileManager.hpp"
#include "MainSingleton/MainSingleton.hpp"

namespace Editor
{
	SceneItemSetAsStart::SceneItemSetAsStart(const std::string& aName) : MainMenuItem(aName)
	{
	}

	void SceneItemSetAsStart::OnClick()
	{
		nlohmann::json jsonData = SimpleUtilities::FileManager::GetDataAsJson(SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_GAME));
		jsonData["Game_Settings"]["Start_Scene_RelativePath"] = MainSingleton::GetSceneManager().GetCurrentSceneInfo()->relativePath;

		std::ofstream writeFile(SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_GAME));
		assert(writeFile.is_open() && "Failed to open the file");

		writeFile << jsonData;
		writeFile.close();
	}

	void SceneItemSetAsStart::ToolTips()
	{
		ImGui::Text("Open current scene on startup");
	}
}
