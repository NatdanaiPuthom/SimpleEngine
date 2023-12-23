#include "Engine/stdafx.h"
#include "Engine/Graphics/Renderer/Renderer.hpp"
#include "Engine/Graphics/Shapes/ShapeCreator3000.hpp"

Renderer::Renderer()
{
	InitTerrain();
	InitSkyBox();
	InitDirectionalLight();
}

Renderer::~Renderer()
{
}

void Renderer::AddMesh(std::unique_ptr<Mesh> aMesh)
{
	myMeshes.emplace_back(std::move(aMesh));
}

void Renderer::Update()
{
	{ //Test Rotation Over Time
		//Pyramid
		SimpleUtilities::Vector3f pyramidRotation = myMeshes[2]->GetRotation();
		pyramidRotation.y += -10 * SimpleGlobal::GetDeltaTime();
		pyramidRotation.z += 10 * SimpleGlobal::GetDeltaTime();
		myMeshes[2]->SetRotation(pyramidRotation);

		//Cube
		SimpleUtilities::Vector3f cubeRotation = myMeshes[3]->GetRotation();
		cubeRotation.x += 10.0f * SimpleGlobal::GetDeltaTime();
		myMeshes[3]->SetRotation(cubeRotation);

		//Directional Light test
		SimpleUtilities::Vector3f directionalLight = SimpleGlobal::GetGraphicsEngine()->GetDirectionalLightDirection() * 180.0f;
		myDirectionalLight->SetRotation(directionalLight);
	}
}

void Renderer::Render()
{
	for (const auto& mesh : myMeshes)
	{
		mesh->Draw();
	}

	myDirectionalLight->Draw();
}

void Renderer::InitTerrain()
{
	std::unique_ptr<Mesh> terrain = std::make_unique<Mesh>();
	MeshData terrainData = ShapeCreator3000::CreateTerrain();

	if (!terrain->Init(terrainData, "Shaders/TerrainPS.cso", "Shaders/TerrainVS.cso"))
		assert(false && "Failed to Init Terrain");

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
	terrain->SetName("Terrain");

	AddMesh(std::move(terrain));
}

void Renderer::InitDirectionalLight()
{
	if (myDirectionalLight != nullptr)
		assert(false && "Already Initialized");

	myDirectionalLight = std::make_unique<Mesh>();

	MeshData directionalLightData = ShapeCreator3000::CreateDirectionalLight();

	if (!myDirectionalLight->Init(directionalLightData))
		assert(false && "Failed to create Directional Light");

	myDirectionalLight->SetPosition(SimpleUtilities::Vector3f(8, 6, 10));
}

void Renderer::InitSkyBox()
{
	std::unique_ptr<Mesh> skybox = std::make_unique<Mesh>();
	MeshData skyboxData = ShapeCreator3000::CreateSkyBox(SimpleUtilities::Vector3f(100, 100, 100));

	if (!skybox->Init(skyboxData, "Shaders/SkyBoxPS.cso", "Shaders/SkyBoxVS.cso"))
		assert(false && "Failed to create SkyBox");

	if (!skybox->AddTexture(14, "Assets/tga/Uppgift7/cubemap.dds"))
		assert(false && "Failed to Add Texture");

	skybox->SetPosition({ 0,0,20 });
	skybox->SetName("SkyBox");

	AddMesh(std::move(skybox));
}

std::vector<Mesh*> Renderer::GetMeshes()
{
	std::vector<Mesh*> meshes;
	
	for (auto& mesh : myMeshes)
	{
		meshes.push_back(mesh.get());
	}

	return meshes;
}
