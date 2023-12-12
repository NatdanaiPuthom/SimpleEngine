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

	if (!myMeshes.emplace_back(std::make_unique<Mesh>())->Init(meshData[0], "Shaders/DefaultPS.cso", "Shaders/DefaultColorfulVS.cso", "Assets/colors.dds"))
		assert(false && "Failed To Create Cube");

	if (!myMeshes.emplace_back(std::make_unique<Mesh>())->Init(meshData[1], "Shaders/DefaultPS.cso", "Shaders/DefaultVS.cso", "Assets/wood.dds"))
		assert(false && "Failed To Create Pyramid");

	if (!myMeshes.emplace_back(std::make_unique<Mesh>())->Init(meshData[2], "Shaders/DefaultPS.cso", "Shaders/Terrain.cso", "Assets/grass.dds"))
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

	if (ImGui::Begin("Camera Controls"))
	{
		ImGui::Text("Movements    - WASD");
		ImGui::Text("Fly Up       - SPACE");
		ImGui::Text("Fly Down     - Ctrl + SPACE");
		ImGui::Text("Rotate Left  - E");
		ImGui::Text("Rotate Right - Q");
		ImGui::Text("Rotate Up    - C");
		ImGui::Text("Rotate Down  - Z");
	}
	ImGui::End();

	if (ImGui::Begin("Frame Counter"))
	{
		std::string fps = "FPS: " + std::to_string(SimplyGlobal::GetFPS());
		ImGui::Text(fps.c_str());
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
