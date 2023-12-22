#include "stdafx.h"
#include "Engine/Graphics/Shapes/ShapeCreator3000.hpp"
#include "Engine/Graphics/Renderer/Renderer.hpp"
#include "Game/GameWorld.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"
#include <External/imgui.h>

GameWorld::GameWorld()
	: myRenderer(std::make_unique<Renderer>())
	, myImGuiManager(std::make_unique<ImGuiManager>())
{
}

GameWorld::~GameWorld()
{
}

void GameWorld::Init()
{
	{ //Test stuff
		std::unique_ptr<Mesh> pyramid = std::make_unique<Mesh>();
		std::unique_ptr<Mesh> cube = std::make_unique<Mesh>();

		MeshData pyramidData = ShapeCreator3000::CreatePyramid();
		MeshData cubeData = ShapeCreator3000::CreateCube();

		if (!pyramid->Init(pyramidData))
			assert(false && "Failed to create Pyramid");

		if (!cube->Init(cubeData))
			assert(false && "Failed to create Cube");

		if (!pyramid->AddTexture(0, "Assets/Textures/Cat.dds"))
			assert(false && "Failed To Add Texture");

		pyramid->SetScale({5,5,5});
		pyramid->SetPosition(SimpleUtilities::Vector3f(-10, 0, 5));
		pyramid->SetName("Pyramid");

		cube->SetScale({5,5,5});
		cube->SetPosition(SimpleUtilities::Vector3f(-5, 0, 5));
		cube->SetName("Cube");

		myRenderer->AddMesh(std::move(pyramid));
		myRenderer->AddMesh(std::move(cube));
	}
}

void GameWorld::Update()
{
	while (SimplyGlobal::GetGameIsRunning())
	{
	}
}

void GameWorld::Render()
{
	if (ImGui::Begin("Meshes")) //TO-DO: Move this to it's own place
	{
		std::vector<Mesh*> meshes = myRenderer->GetMeshes();
		std::vector<std::string> meshNames;

		for (const Mesh* mesh : meshes)
		{
			meshNames.push_back(mesh->GetName());
		}

		static int selectedMeshIndex = 0;

		ImGui::SetNextItemWidth(200);
		if (ImGui::Combo("Meshes", &selectedMeshIndex, meshNames.data()->c_str(), static_cast<int>(meshNames.size())))
		{
		}

		ImGui::SetNextItemWidth(400);
		SimpleUtilities::Vector3f position = meshes[selectedMeshIndex]->GetPosition();
		if (ImGui::SliderFloat3("Position", &position.x, -10.0f, 20.0f))
		{
			meshes[selectedMeshIndex]->SetPosition(position);
		}

		ImGui::SetNextItemWidth(400);
		SimpleUtilities::Vector3f rotation = meshes[selectedMeshIndex]->GetRotation();
		if (ImGui::SliderFloat3("Rotation", &rotation.x, 0.0f, 360.0f))
		{
			meshes[selectedMeshIndex]->SetRotation(rotation);
		}

		ImGui::SetNextItemWidth(400);
		SimpleUtilities::Vector3f scale = meshes[selectedMeshIndex]->GetScale();
		if (ImGui::SliderFloat3("Scale", &scale.x, 1.0f, 20.0f))
		{
			meshes[selectedMeshIndex]->SetScale(scale);
		}
	}
	ImGui::End();

	myRenderer->Render();
	myImGuiManager->Render();
}
