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

	if (!myMeshes.emplace_back(std::make_unique<Mesh>())->Init(meshData[0], "DefaultPS.cso", "DefaultColorfulVS.cso", "colors.dds"))
		assert(false && "Failed To Create Cube");

	if (!myMeshes.emplace_back(std::make_unique<Mesh>())->Init(meshData[1], "DefaultPS.cso", "DefaultVS.cso", "wood.dds"))
		assert(false && "Failed To Create Pyramid");

	if (!myMeshes.emplace_back(std::make_unique<Mesh>())->Init(meshData[2], "DefaultPS.cso", "Terrain.cso", "grass.dds"))
		assert(false && "Failed To Create Terrain");

	myMeshes[0]->SetPosition(SimpleUtilities::Vector3f(-3, 0, 0));
	myMeshes[1]->SetPosition(SimpleUtilities::Vector3f(-1, 0, 0));
	myMeshes[2]->SetPosition(SimpleUtilities::Vector3f(0, 0, 0));
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
