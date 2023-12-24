#include "Game/stdafx.h"
#include "Engine/Graphics/Renderer/Renderer.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"
#include "Game/Managers/ImGuiManager/Tools/CameraTool.hpp"
#include "Game/Managers/ImGuiManager/Tools/StatsTool.hpp"
#include "Game/Managers/ImGuiManager/Tools/LightTool.hpp"
#include "Game/Managers/ImGuiManager/Tools/SceneTool.hpp"

ImGuiManager::ImGuiManager() 
	: myRenderer(nullptr)
{
	AddTool(std::move(std::make_unique<CameraTool>()));
	AddTool(std::move(std::make_unique<StatsTool>()));
	AddTool(std::move(std::make_unique<LightTool>()));
	AddTool(std::move(std::make_unique<SceneTool>()));
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

void ImGuiManager::SetRenderer(Renderer* aRenderer)
{
	myRenderer = aRenderer;
}
