#include "Game/Precomplier/stdafx.h"
#include "Engine/Graphics/Renderer/Renderer.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"
#include "Game/Managers/ImGuiManager/Tools/CameraTool.hpp"
#include "Game/Managers/ImGuiManager/Tools/SettingsTool.hpp"
#include "Game/Managers/ImGuiManager/Tools/LightTool.hpp"
#include "Game/Managers/ImGuiManager/Tools/SceneTool.hpp"
#include "Game/Managers/ImGuiManager/Tools/MeshTool.hpp"
#include "Game/Managers/ImGuiManager/Tools/DebugTool.hpp"
#include "Game/Managers/ImGuiManager/Tools/PlaneReflectionTool.hpp"
#include "Game/Managers/ImGuiManager/Menu/MainMenuBar.hpp"

namespace Simple
{
	ImGuiManager::ImGuiManager()
	{
		AddTool(std::move(std::make_unique<Tool::LightTool>()));
		AddTool(std::move(std::make_unique<Tool::SceneTool>()));
		AddTool(std::move(std::make_unique<Tool::MeshTool>()));
		AddTool(std::move(std::make_unique<Tool::DebugTool>()));
		AddTool(std::move(std::make_unique<Tool::SettingsTool>()));
		AddTool(std::move(std::make_unique<Tool::PlaneReflectionTool>()));
		AddTool(std::move(std::make_unique<Tool::MainMenuBar>()));
		AddTool(std::move(std::make_unique<Tool::CameraTool>()));
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
		for (const std::unique_ptr<Simple::ToolInterface>& tool : myTools)
		{
			tool->Draw();
		}
	}
}