#include "stdafx.h"
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

void Renderer::Render()
{
	{ //Test Rotation Over Time
		//Pyramid
		SimpleUtilities::Vector3f pyramidRotation = myMeshes[0]->GetRotation();
		pyramidRotation.y += -10 * SimplyGlobal::GetDeltaTime();
		myMeshes[0]->SetRotation(pyramidRotation);

		//Cube
		SimpleUtilities::Vector3f cubeRotation = myMeshes[1]->GetRotation();
		cubeRotation.x += 10 * SimplyGlobal::GetDeltaTime();
		cubeRotation.z += 10 * SimplyGlobal::GetDeltaTime();
		myMeshes[1]->SetRotation(cubeRotation);

		//Directional Light test
		SimpleUtilities::Vector3f directionalLight = SimplyGlobal::GetGraphicsEngine()->GetDirectionalLightDirection() * 180.0f;
		myDirectionalLight->SetRotation(directionalLight);
	}

	for (const auto& mesh : myMeshes)
	{
		mesh->Draw();
	}

	mySkyBox->Draw();
	myDirectionalLight->Draw();
	myTerrain->Draw();
}

void Renderer::InitTerrain()
{
	if (myTerrain != nullptr)
		assert(false && "Already Initialized");

	myTerrain = std::make_unique<Mesh>();

	MeshData terrainData = ShapeCreator3000::CreateTerrain();

	if (!myTerrain->Init(terrainData, "Shaders/TerrainPS.cso", "Shaders/TerrainVS.cso"))
		assert(false && "Failed to Init Terrain");

	if (!myTerrain->AddTexture(1, "Assets/tga/Uppgift6/Grass_c.dds"))
		assert(false && "Failed to Add Texture");

	if (!myTerrain->AddTexture(2, "Assets/tga/Uppgift6/Rock_c.dds"))
		assert(false && "Failed to Add Texture");

	if (!myTerrain->AddTexture(3, "Assets/tga/Uppgift6/Snow_c.dds"))
		assert(false && "Failed to Add Texture");

	if (!myTerrain->AddTexture(4, "Assets/tga/Uppgift6/Grass_n.dds"))
		assert(false && "Failed to Add Texture");

	if (!myTerrain->AddTexture(5, "Assets/tga/Uppgift6/Rock_n.dds"))
		assert(false && "Failed to Add Texture");

	if (!myTerrain->AddTexture(6, "Assets/tga/Uppgift6/Snow_n.dds"))
		assert(false && "Failed to Add Texture");

	if (!myTerrain->AddTexture(7, "Assets/tga/Uppgift7/Grass_m.dds"))
		assert(false && "Failed to Add Texture");

	if (!myTerrain->AddTexture(8, "Assets/tga/Uppgift7/Rock_m.dds"))
		assert(false && "Failed to Add Texture");

	if (!myTerrain->AddTexture(9, "Assets/tga/Uppgift7/Snow_m.dds"))
		assert(false && "Failed to Add Texture");

	myTerrain->SetPosition(SimpleUtilities::Vector3f(-3, 0, 0));
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
	if (mySkyBox != nullptr)
		assert(false && "Already Initialized");

	mySkyBox = std::make_unique<Mesh>();

	MeshData skyboxData = ShapeCreator3000::CreateSkyBox(SimpleUtilities::Vector3f(100, 100, 100));

	if (!mySkyBox->Init(skyboxData, "Shaders/SkyBoxPS.cso", "Shaders/SkyBoxVS.cso"))
		assert(false && "Failed to create SkyBox");

	if (!mySkyBox->AddTexture(14, "Assets/tga/Uppgift7/cubemap.dds"))
		assert(false && "Failed to Add Texture");

	mySkyBox->SetPosition({ 0,0,20 });
}