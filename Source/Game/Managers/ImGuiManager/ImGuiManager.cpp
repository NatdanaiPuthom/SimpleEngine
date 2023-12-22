#include "Game/stdafx.h"
#include "Engine/Graphics/Renderer/Renderer.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"

ImGuiManager::ImGuiManager() 
	: myRenderer(nullptr)
{
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
