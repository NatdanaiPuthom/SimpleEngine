#include "stdafx.h"
#include "Game/GameWorld.h"
#include "Engine/Graphics/Model/Model.h"
#include "Engine/Graphics/Shapes/Cube.h"
#include "Engine/Graphics/Shapes/Pyramid.h"
#include "Engine/Graphics/GraphicsEngine.h"
#include <External/imgui.h>

GameWorld::GameWorld()
{
}

GameWorld::~GameWorld()
{
}

void GameWorld::Init()
{
	myModels.emplace_back(std::make_unique<Cube>());
	myModels.emplace_back(std::make_unique<Pyramid>());

	for (auto& model : myModels)
	{
		if (model->Create() == false)
			assert(false && "Failed to create model");
	}
}

void GameWorld::Render()
{
	for (const auto& model : myModels)
	{
		model->Draw();
	}

	if (ImGui::Begin("My Window"))
	{
		ImGui::Text("Hello World");
	}
	ImGui::End();

	if (ImGui::Begin("Scene"))
	{
		ImVec2 size(1280, 720);
		const ImTextureID textureID = reinterpret_cast<ImTextureID>(SimplyGlobal::GetGraphicsEngine()->GetShaderResourceView().Get());
		ImGui::Image(textureID, size);
	}
	ImGui::End();
}
