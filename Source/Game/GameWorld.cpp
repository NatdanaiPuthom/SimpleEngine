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
	std::unique_ptr<Mesh> terrain = std::make_unique<Mesh>();
	std::unique_ptr<Mesh> pyramid = std::make_unique<Mesh>();
	std::unique_ptr<Mesh> cube = std::make_unique<Mesh>();

	MeshData terrainData = Shape::CreateTerrain();
	MeshData pyramidData = Shape::CreatePyramid();
	MeshData cubeData = Shape::CreateCube();

	if (!terrain->Init(terrainData, "Shaders/TerrainPS.cso", "Shaders/TerrainVS.cso"))
		assert(false && "Failed to Init Terrain");

	if (!pyramid->Init(pyramidData))
		assert(false && "Failed to Pyramid");

	if (!cube->Init(cubeData))
		assert(false && "Failed to Cube");

	terrain->SetPosition(SimpleUtilities::Vector3f(-3, 0, 0));
	pyramid->SetPosition(SimpleUtilities::Vector3f(-5, 0, 0));
	cube->SetPosition(SimpleUtilities::Vector3f(-5, 0, 5));

	myRenderer->AddMesh(std::move(terrain));
	myRenderer->AddMesh(std::move(pyramid));
	myRenderer->AddMesh(std::move(cube));
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
		{
			graphicsEngine->GetCamera()->SetMoveSpeed(speed);
		}

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
		{
			graphicsEngine->SetVSync(vsync);
		}
	}
	ImGui::End();

	if (ImGui::Begin("Directional Light"))
	{
		ImGui::SetNextItemWidth(400.0f);

		SimpleUtilities::Vector3f direction = graphicsEngine->GetDirectionalLightDirection();
		if (ImGui::SliderFloat3("Direction", &direction.x, -1.0f, 1.0f, "%.03f"))
		{
			graphicsEngine->SetDirectionalLightDirection(direction);
		}
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
