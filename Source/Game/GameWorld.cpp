#include "stdafx.h"
#include "Engine/Graphics/Shapes/ShapeCreator3000.hpp"
#include "Engine/Graphics/Renderer/Renderer.hpp"
#include "Game/GameWorld.hpp"
#include "Game/Managers/ImGuiManager/ImGuiManager.hpp"

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
		std::unique_ptr<Mesh> terrain = std::make_unique<Mesh>();
		std::unique_ptr<Mesh> pyramid = std::make_unique<Mesh>();
		std::unique_ptr<Mesh> cube = std::make_unique<Mesh>();
		std::unique_ptr<Mesh> directionalLight = std::make_unique<Mesh>();
		std::unique_ptr<Mesh> skybox = std::make_unique<Mesh>();

		MeshData terrainData = Shape::CreateTerrain();
		MeshData pyramidData = Shape::CreatePyramid();
		MeshData cubeData = Shape::CreateCube();
		MeshData directionalLightData = Shape::CreateDirectionalLight();
		MeshData skyboxData = Shape::CreateSkyBox(SimpleUtilities::Vector3f(100, 100, 100));

		if (!terrain->Init(terrainData, "Shaders/TerrainPS.cso", "Shaders/TerrainVS.cso"))
			assert(false && "Failed to Init Terrain");

		if (!pyramid->Init(pyramidData))
			assert(false && "Failed to create Pyramid");

		if (!cube->Init(cubeData))
			assert(false && "Failed to create Cube");

		if (!directionalLight->Init(directionalLightData))
			assert(false && "Failed to create Directional Light");

		if (!skybox->Init(skyboxData, "Shaders/SkyBoxPS.cso", "Shaders/SkyBoxVS.cso"))
			assert(false && "Failed to create SkyBox");

		if (!skybox->AddTexture(14, "Assets/tga/Uppgift7/cubemap.dds"))
			assert(false && "Failed to Add Texture");

		if (!terrain->AddTexture(1, "Assets/tga/Uppgift6/Grass_c.dds"))
			assert(false && "Failed to Add Texture");

		if (!terrain->AddTexture(2, "Assets/tga/Uppgift6/Rock_c.dds"))
			assert(false && "Failed to Add Texture");

		if (!terrain->AddTexture(3, "Assets/tga/Uppgift6/Snow_c.dds"))
			assert(false && "Failed to Add Texture");

		if (!terrain->AddTexture(4, "Assets/tga/Uppgift6/Grass_n.dds"))
			assert(false && "Failed to Add Texture");

		if (!terrain->AddTexture(5, "Assets/tga/Uppgift6/Rock_n.dds"))
			assert(false && "Failed to Add Texture");

		if (!terrain->AddTexture(6, "Assets/tga/Uppgift6/Snow_n.dds"))
			assert(false && "Failed to Add Texture");

		if (!terrain->AddTexture(7, "Assets/tga/Uppgift7/Grass_m.dds"))
			assert(false && "Failed to Add Texture");

		if (!terrain->AddTexture(8, "Assets/tga/Uppgift7/Rock_m.dds"))
			assert(false && "Failed to Add Texture");

		if (!terrain->AddTexture(9, "Assets/tga/Uppgift7/Snow_m.dds"))
			assert(false && "Failed to Add Texture");

		terrain->SetPosition(SimpleUtilities::Vector3f(-3, 0, 0));
		pyramid->SetPosition(SimpleUtilities::Vector3f(-5, 0, 0));
		cube->SetPosition(SimpleUtilities::Vector3f(-5, 0, 5));
		directionalLight->SetPosition(SimpleUtilities::Vector3f(8, 6, 10));
		skybox->SetPosition({ 0,0,20 });

		myRenderer->AddMesh(std::move(terrain));
		myRenderer->AddMesh(std::move(pyramid));
		myRenderer->AddMesh(std::move(cube));
		myRenderer->AddMesh(std::move(directionalLight));
		myRenderer->AddMesh(std::move(skybox));
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
	myRenderer->Render();
	myImGuiManager->Render();	
}
