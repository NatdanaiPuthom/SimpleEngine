#include "Game/Precomplier/stdafx.h"
#include "Engine/Graphics/Renderer/Renderer.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"
#include "Game/Managers/ImGuiManager/Tools/CameraTool.hpp"
#include "Game/Managers/ImGuiManager/Tools/SettingsTool.hpp"
#include "Game/Managers/ImGuiManager/Tools/LightTool.hpp"
#include "Game/Managers/ImGuiManager/Tools/SceneTool.hpp"
#include "Game/Managers/ImGuiManager/Tools/MeshTool.hpp"
#include "Game/Managers/ImGuiManager/Tools/Debug.hpp"

namespace Simple
{
	ImGuiManager::ImGuiManager()
	{
		AddTool(std::move(std::make_unique<Tool::Light>()));
		AddTool(std::move(std::make_unique<Tool::Scene>()));
		AddTool(std::move(std::make_unique<Tool::Mesh>()));
		AddTool(std::move(std::make_unique<Tool::Debug>()));
		AddTool(std::move(std::make_unique<Tool::Settings>()));
		AddTool(std::move(std::make_unique<Tool::Camera>()));
	}

	ImGuiManager::~ImGuiManager()
	{
	}

	void ImGuiManager::AddTool(std::unique_ptr<Simple::ToolInterface> aTool)
	{
		myTools.push_back(std::move(aTool));
	}

	void ImGuiManager::Render()
	{
		for (const std::unique_ptr<ToolInterface>& tool : myTools)
		{
			tool->Draw();
		}
	}
}