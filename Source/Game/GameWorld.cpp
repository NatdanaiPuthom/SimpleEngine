#include "stdafx.h"
#include "Game/GameWorld.hpp"
#include "Engine/Graphics/Model/Mesh.hpp"
#include "Engine/Graphics/Shapes/ShapeCreator3000.hpp"
#include "Engine/Graphics/GraphicsEngine.hpp"
#include "Engine/Graphics/Renderer/Renderer.hpp"
#include "Engine/Graphics/Camera/Camera.hpp"
#include <External/imgui.h>

GameWorld::GameWorld()
	: myRenderer(std::make_unique<Renderer>())
{
}

GameWorld::~GameWorld()
{
}

void GameWorld::Init()
{
	MeshData meshData = Shape::CreateTerrain();
	std::unique_ptr<Mesh>  mesh = std::make_unique<Mesh>();

	if (!mesh->Init(meshData, "Shaders/TerrainPS.cso", "Shaders/TerrainVS.cso"))
		assert(false && "Failed to Init Mesh");

	mesh->SetPosition(SimpleUtilities::Vector3f(-3, 0, 0));

	myRenderer->AddMesh(std::move(mesh));
}

void GameWorld::Update()
{
	while (SimplyGlobal::GetGameIsRunning())
	{
	}
}

void GameWorld::Render()
{
	GraphicsEngine* graphicsEngine = SimplyGlobal::GetGraphicsEngine();

	myRenderer->Render();

	if (ImGui::Begin("Camera Controls"))
	{
		ImGui::Text("Movements    - WASD");
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(200, 0));
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100.0f);
		float speed = graphicsEngine->GetCamera()->GetMoveSpeed();
		if (ImGui::SliderFloat("Speed", &speed, 0, 100, "%0.2f"))
			graphicsEngine->GetCamera()->SetMoveSpeed(speed);

		ImGui::Text("FreeFly      - TAB");
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

		bool vsync = graphicsEngine->IsVSyncActive();
		if (ImGui::Checkbox("VSync", &vsync))
			graphicsEngine->SetVSync(vsync);
	}
	ImGui::End();

	/*if (ImGui::Begin("Scene"))
	{
		ImVec2 size(1280, 720);
		const ImTextureID textureID = graphicsEngine->GetShaderResourceView().Get();
		ImGui::Image(textureID, size);
	}
	ImGui::End();*/
}
