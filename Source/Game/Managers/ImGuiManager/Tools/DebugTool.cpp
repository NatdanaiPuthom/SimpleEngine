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

			WaterPlane();
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

	void DebugTool::WaterPlane()
	{
		auto& waterPlane = SimpleWorld::GetWaterPlane()->myModel;

		std::vector<std::string> modelNames;
		modelNames.push_back(waterPlane->GetName());

		std::vector<const char*> names;
		for (const std::string& name : modelNames)
		{
			names.push_back(name.c_str());
		}

		int selectedWaterIndex = 0;
		ImGui::SetNextItemWidth(200);
		if (ImGui::Combo("Plane", &selectedWaterIndex, names.data(), static_cast<int>(names.size())))
		{
		}

		ImGui::Dummy(ImVec2(0, 10));
		ImGui::SetNextItemWidth(400);
		SimpleUtilities::Vector3f position = waterPlane->GetPosition();
		if (ImGui::DragFloat3("Position", &position.x, 0.1f))
		{
			waterPlane->SetPosition(position);
		}

		ImGui::SetNextItemWidth(400);
		SimpleUtilities::Vector3f rotation = waterPlane->GetRotation();
		if (ImGui::DragFloat3("Rotation", &rotation.x, 1.0f, 0.0f, 360.0f, "%.3f"))
		{
			waterPlane->SetRotation(rotation);
		}

		ImGui::SetNextItemWidth(400);
		SimpleUtilities::Vector3f scale = waterPlane->GetScale();
		if (ImGui::DragFloat3("Scale", &scale.x, 0.01f, 0.001f, 100.0f, "%0.3f"))
		{
			waterPlane->SetScale(scale);
		}
	}
}