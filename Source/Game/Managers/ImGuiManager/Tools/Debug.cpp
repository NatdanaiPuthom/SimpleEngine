#include "Game/Precomplier/stdafx.h"
#include "Game/Managers/ImGuiManager/Tools/Debug.hpp"

namespace Tool
{
	Debug::Debug()
	{
		LoadDataFromJson();
	}

	Debug::~Debug()
	{
	}

	void Debug::Draw()
	{
		if (ImGui::Begin("Debug"))
		{
			auto renderer = SimpleGlobal::GetRenderer();

			bool debugMode = renderer->IsDebugModeOn();
			if (ImGui::Checkbox("Draw BoundingBox", &debugMode))
			{
				renderer->SetDebugMode(debugMode);
			}

			std::vector<std::string> sceneIndexStrings;
			sceneIndexStrings.reserve(mySceneIndexes.size());
			for (const int index : mySceneIndexes)
			{
				sceneIndexStrings.push_back(std::to_string(index));
			}

			std::vector<const char*> sceneIndexChar;
			sceneIndexChar.reserve(mySceneIndexes.size());
			for (const std::string& indexString : sceneIndexStrings)
			{
				sceneIndexChar.push_back(indexString.c_str());
			}

			ImGui::SetNextItemWidth(200);
			myActiveScene = SimpleWorld::GetActiveSceneIndex();
			if (ImGui::Combo("Active Scene", &myActiveScene, sceneIndexChar.data(), static_cast<int>(sceneIndexChar.size())))
			{
				SimpleWorld::SetActiveScene(mySceneIndexes[myActiveScene]);
			}
		}
		ImGui::End();
	}

	void Debug::LoadDataFromJson()
	{
		const std::string filename = SimpleUtilities::GetPath(SIMPLE_LEVELS_FILENAME);
		std::ifstream file(filename);
		assert(file.is_open() && "Failed To Open File");

		const nlohmann::json json = nlohmann::json::parse(file);
		file.close();

		const nlohmann::json& scenesIndexes = json["scenes"];

		for (const auto& index : scenesIndexes)
		{
			mySceneIndexes.push_back(index["id"]);
		}
	}
}