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

ImGuiManager::ImGuiManager()
{
	AddTool(std::move(std::make_unique<CameraTool>()));
	AddTool(std::move(std::make_unique<SettingsTool>()));
	AddTool(std::move(std::make_unique<LightTool>()));
	AddTool(std::move(std::make_unique<SceneTool>()));
	AddTool(std::move(std::make_unique<MeshTool>()));
	AddTool(std::move(std::make_unique<DebugTool>()));
}

ImGuiManager::~ImGuiManager()
{
}

void ImGuiManager::AddTool(std::unique_ptr<ToolInterface> aTool)
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
