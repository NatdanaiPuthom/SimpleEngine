#include "stdafx.h"
#include "Game/GameWorld.h"
#include "Engine/Graphics/Model/Mesh.h"
#include "Engine/Graphics/Shapes/ShapeCreator3000.h"
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
	std::vector<MeshData> meshData;
	meshData.emplace_back(Shape::CreateCube());
	meshData.emplace_back(Shape::CreatePyramid());
	meshData.emplace_back(Shape::CreateTerrain());

	for (size_t i = 0; i < 2; ++i)
	{
		if (!myMeshes.emplace_back(std::make_unique<Mesh>())->Init(meshData[i], "DefaultPS.cso", "DefaultColorfulVS.cso", "grass.dds"))
			assert(false && "Failed To Create Mesh");
	}

	if (!myMeshes.emplace_back(std::make_unique<Mesh>())->Init(meshData[2], "DefaultPS.cso", "DefaultVS.cso", "grass.dds"))
		assert(false && "Failed To Create Mesh");
}

void GameWorld::Render()
{
	for (const auto& model : myMeshes)
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
		const ImTextureID textureID = SimplyGlobal::GetGraphicsEngine()->GetShaderResourceView().Get();
		ImGui::Image(textureID, size);
	}
	ImGui::End();
}
