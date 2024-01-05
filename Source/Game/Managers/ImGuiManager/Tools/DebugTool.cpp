#include "Game/Precomplier/stdafx.h"
#include "Game/Managers/ImGuiManager/Tools/DebugTool.hpp"

namespace Tool
{
	DebugTool::DebugTool()
	{
		LoadDataFromJson();
	}

	DebugTool::~DebugTool()
	{
	}

	void DebugTool::Draw()
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
			sceneIndexStrings.push_back(std::to_string(0) + std::string(" Water Reflection Test"));
			sceneIndexStrings.push_back(std::to_string(1) + std::string(" Spotlights Test"));
			sceneIndexStrings.push_back(std::to_string(2) + std::string(" Navmesh Test"));

			std::vector<const char*> sceneNameChar;
			sceneNameChar.reserve(mySceneIndexes.size());
			for (const std::string& indexString : sceneIndexStrings)
			{
				sceneNameChar.push_back(indexString.c_str());
			}

			ImGui::SetNextItemWidth(200);
			myActiveScene = SimpleWorld::GetActiveSceneIndex();
			if (ImGui::Combo("Active Scene", &myActiveScene, sceneNameChar.data(), static_cast<int>(sceneNameChar.size())))
			{
				SimpleWorld::SetActiveScene(mySceneIndexes[myActiveScene]);
			}
		}

		ImGui::End();
	}

	void DebugTool::LoadDataFromJson()
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