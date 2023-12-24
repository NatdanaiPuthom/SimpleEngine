#include "Engine/stdafx.h"
#include "Engine/Graphics/Renderer/Renderer.hpp"

Renderer::Renderer()
{
	InitShapes();

	InitTerrain();
	InitSkyBox();
	InitDirectionalLight();
}

Renderer::~Renderer()
{
}

void Renderer::AddMesh(const std::string& aName, std::unique_ptr<Mesh> aMesh)
{
	myMeshes.emplace(aName, std::move(aMesh));
}

void Renderer::AddModelInstance(std::unique_ptr<ModelInstance> aModelInstance)
{
	myModelInstances.push_back(std::move(aModelInstance));
}

void Renderer::Update()
{
	{ //Test Rotation Over Time
		//Pyramid
		SimpleUtilities::Vector3f pyramidRotation = myModelInstances[2]->GetRotation();
		pyramidRotation.y += -10 * SimpleGlobal::GetDeltaTime();
		pyramidRotation.z += 10 * SimpleGlobal::GetDeltaTime();
		myModelInstances[2]->SetRotation(pyramidRotation);

		//Cube
		SimpleUtilities::Vector3f cubeRotation = myModelInstances[3]->GetRotation();
		cubeRotation.x += 10.0f * SimpleGlobal::GetDeltaTime();
		myModelInstances[3]->SetRotation(cubeRotation);

		//Directional Light test
		SimpleUtilities::Vector3f directionalLight = SimpleGlobal::GetGraphicsEngine()->GetDirectionalLightDirection() * 180.0f;
		myDirectionalLight->SetRotation(directionalLight);
	}
}

void Renderer::Render()
{
	for (const auto& model : myModelInstances)
	{
		model->Render();
	}

	myDirectionalLight->Render();
}

void Renderer::InitTerrain()
{
	std::unique_ptr<ModelInstance> terrainModel = std::make_unique<ModelInstance>();
	terrainModel->Init(GetMesh("Terrain"));
	terrainModel->ClearTextures();
	terrainModel->SetShader("Shaders/TerrainPS.cso", "Shaders/TerrainVS.cso");

	if (!terrainModel->AddTexture("Assets/tga/Uppgift6/Grass_c.dds"))
		assert(false && "Failed to Add Texture");

	if (!terrainModel->AddTexture("Assets/tga/Uppgift6/Rock_c.dds"))
		assert(false && "Failed to Add Texture");

	if (!terrainModel->AddTexture("Assets/tga/Uppgift6/Snow_c.dds"))
		assert(false && "Failed to Add Texture");

	if (!terrainModel->AddTexture("Assets/tga/Uppgift6/Grass_n.dds"))
		assert(false && "Failed to Add Texture");

	if (!terrainModel->AddTexture("Assets/tga/Uppgift6/Rock_n.dds"))
		assert(false && "Failed to Add Texture");

	if (!terrainModel->AddTexture("Assets/tga/Uppgift6/Snow_n.dds"))
		assert(false && "Failed to Add Texture");

	if (!terrainModel->AddTexture("Assets/tga/Uppgift7/Grass_m.dds"))
		assert(false && "Failed to Add Texture");

	if (!terrainModel->AddTexture("Assets/tga/Uppgift7/Rock_m.dds"))
		assert(false && "Failed to Add Texture");

	if (!terrainModel->AddTexture("Assets/tga/Uppgift7/Snow_m.dds"))
		assert(false && "Failed to Add Texture");

	terrainModel->SetPosition(SimpleUtilities::Vector3f(-3, 0, 0));
	terrainModel->SetName("Terrain");

	AddModelInstance(std::move(terrainModel));
}

void Renderer::InitDirectionalLight()
{
	myDirectionalLight = std::make_unique<ModelInstance>();
	myDirectionalLight->Init(GetMesh("DirectionalLight"));
	myDirectionalLight->SetPosition(SimpleUtilities::Vector3f(8, 6, 10));
	myDirectionalLight->SetName("DirectionalLight");
}

void Renderer::InitSkyBox()
{
	std::unique_ptr<ModelInstance> skyboxModel = std::make_unique<ModelInstance>();

	skyboxModel->Init(GetMesh("Skybox"), "Assets/tga/Uppgift7/cubemap.dds");
	skyboxModel->SetShader("Shaders/SkyBoxPS.cso", "Shaders/SkyBoxVS.cso");
	skyboxModel->SetPosition({ 0,0,20 });
	skyboxModel->SetName("SkyBox");

	AddModelInstance(std::move(skyboxModel));
}

void Renderer::InitShapes()
{
	MeshData cubeData = ShapeCreator3000::CreateCube();
	MeshData pyramidData = ShapeCreator3000::CreatePyramid();
	MeshData planeData = ShapeCreator3000::CreatePlane();
	MeshData skyboxData = ShapeCreator3000::CreateSkyBox(SimpleUtilities::Vector3f(100, 100, 100));
	MeshData directionalLightData = ShapeCreator3000::CreateDirectionalLight();
	MeshData terrainData = ShapeCreator3000::CreateTerrain();

	std::unique_ptr<Mesh> cube = std::make_unique<Mesh>();
	std::unique_ptr<Mesh> pyramid = std::make_unique<Mesh>();
	std::unique_ptr<Mesh> plane = std::make_unique<Mesh>();
	std::unique_ptr<Mesh> skyboxMesh = std::make_unique<Mesh>();
	std::unique_ptr<Mesh> directionalLight = std::make_unique<Mesh>();
	std::unique_ptr<Mesh> terrainMesh = std::make_unique<Mesh>();

	if (!cube->Init(cubeData))
		assert(false && "Failed to create Cube");

	if (!pyramid->Init(pyramidData))
		assert(false && "Failed to create Pyramid");

	if (!plane->Init(planeData))
		assert(false && "Failed to create Plane");

	if (!skyboxMesh->Init(skyboxData))
		assert(false && "Failed to create SkyBox");

	if (!directionalLight->Init(directionalLightData))
		assert(false && "Failed to create Directional Light");

	if (!terrainMesh->Init(terrainData))
		assert(false && "Failed to create Terrain");

	AddMesh("Cube", std::move(cube));
	AddMesh("Pyramid", std::move(pyramid));
	AddMesh("Plane", std::move(plane));
	AddMesh("Skybox", std::move(skyboxMesh));
	AddMesh("DirectionalLight", std::move(directionalLight));
	AddMesh("Terrain", std::move(terrainMesh));
}

Mesh* Renderer::GetMesh(const std::string& aMeshName)
{
	auto it = myMeshes.find(aMeshName);

	if (it != myMeshes.end())
		return it->second.get();

	return nullptr;
}

std::vector<ModelInstance*> Renderer::GetAllModelInstances()
{
	std::vector<ModelInstance*> modelInstances;

	for (auto& model : myModelInstances)
	{
		modelInstances.push_back(model.get());
	}

	return modelInstances;
}
