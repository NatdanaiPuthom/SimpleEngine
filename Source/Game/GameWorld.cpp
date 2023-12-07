#include "Game/GameWorld.h"
#include "Engine/Graphics/Model/Model.h"
#include "Engine/Graphics/Shapes/Cube.h"
#include "Engine/Graphics/Shapes/Pyramid.h"
#include <External/imgui.h>

GameWorld::GameWorld()
{
	myModels.emplace_back(std::make_unique<Cube>());
	myModels.emplace_back(std::make_unique<Pyramid>());

	for (auto& model : myModels)
	{
		if (model->Create() == false)
			assert(false && "Failed to create model");
	}
}

GameWorld::~GameWorld()
{
}

void GameWorld::Render()
{
	for (const auto& model : myModels)
	{
		model->Draw();
	}

	if (ImGui::Begin("My Window", nullptr))
	{
		ImGui::Text("Hello World");
	}
	ImGui::End();
}
