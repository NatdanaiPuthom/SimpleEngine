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

	/*if (ImGui::Begin("Scene")) //Disabled due to issue with ImGui::Image rendering
	{
		ImVec2 size(1280, 720);
		const ImTextureID textureID = graphicsEngine->GetShaderResourceView().Get();
		ImGui::Image(textureID, size);
	}
	ImGui::End();*/
}

void ImGuiManager::SetRenderer(Renderer* aRenderer)
{
	myRenderer = aRenderer;
}
