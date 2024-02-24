#include "Game/Precomplied/GamePch.hpp"
#include "Graphics/Renderer/Renderer.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"
#include "Game/Managers/ImGuiManager/Menu/MainMenuBar.hpp"

namespace Simple
{
	ImGuiManager::ImGuiManager()
	{
		AddTool(std::move(std::make_unique<Tool::MainMenuBar>()));
	}

	ImGuiManager::~ImGuiManager()
	{
	}

	void ImGuiManager::Init()
	{
		for (const auto& tool : myTools)
		{
			tool->Init();
		}
	}

	void ImGuiManager::Render()
	{
		for (const std::unique_ptr<Simple::ToolInterface>& tool : myTools)
		{
			tool->Draw();
		}
	}

	void ImGuiManager::AddTool(std::unique_ptr<Simple::ToolInterface> aTool)
	{
		myTools.push_back(std::move(aTool));
	}
}