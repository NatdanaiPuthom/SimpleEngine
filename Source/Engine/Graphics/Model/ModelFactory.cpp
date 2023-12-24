#include "Engine/stdafx.h"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"

ModelFactory::ModelFactory()
{
	SimpleGlobalModelFactoryImpl::SetModelFactory(this);

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

ModelFactory::~ModelFactory()
{
}

void ModelFactory::AddMesh(const char* aName, std::unique_ptr<Mesh> aMesh)
{
	myMeshes.emplace(aName, std::move(aMesh));
}

Mesh* ModelFactory::GetMesh(const char* aMeshName)
{
	auto mesh = myMeshes.find(aMeshName);

	if (mesh != myMeshes.end())
		return mesh->second.get();

	return nullptr;
}

std::unique_ptr<ModelInstance> ModelFactory::CreateTerrainModel()
{
	std::unique_ptr<ModelInstance> terrainModel = std::make_unique<ModelInstance>();

	terrainModel->Init(GetMesh("Terrain"));
	terrainModel->ClearTextures();
	terrainModel->SetShader("Shaders/TerrainPS.cso", "Shaders/TerrainVS.cso");

	terrainModel->AddTexture("Assets/tga/Uppgift6/Grass_c.dds");
	terrainModel->AddTexture("Assets/tga/Uppgift6/Rock_c.dds");
	terrainModel->AddTexture("Assets/tga/Uppgift6/Snow_c.dds");
	terrainModel->AddTexture("Assets/tga/Uppgift6/Grass_n.dds");
	terrainModel->AddTexture("Assets/tga/Uppgift6/Rock_n.dds");
	terrainModel->AddTexture("Assets/tga/Uppgift6/Snow_n.dds");
	terrainModel->AddTexture("Assets/tga/Uppgift7/Grass_m.dds");
	terrainModel->AddTexture("Assets/tga/Uppgift7/Rock_m.dds");
	terrainModel->AddTexture("Assets/tga/Uppgift7/Snow_m.dds");

	terrainModel->SetScale({ 1,1,1 });
	terrainModel->SetPosition(SimpleUtilities::Vector3f(0, 0, 0));
	terrainModel->SetName("Terrain");

	return std::move(terrainModel);
}

std::unique_ptr<ModelInstance> ModelFactory::CreateSkyBoxModel()
{
	std::unique_ptr<ModelInstance> skyBoxModel = std::make_unique<ModelInstance>();

	skyBoxModel->Init(GetMesh("Skybox"), "Assets/tga/Uppgift7/cubemap.dds");
	skyBoxModel->SetShader("Shaders/SkyBoxPS.cso", "Shaders/SkyBoxVS.cso");
	skyBoxModel->SetScale({ 1,1,1 });
	skyBoxModel->SetPosition({ 0,0,0 });
	skyBoxModel->SetName("SkyBox");

	return std::move(skyBoxModel);
}

std::unique_ptr<ModelInstance> ModelFactory::CreateDirectionalLightModel()
{
	std::unique_ptr<ModelInstance> directionalLight = std::make_unique<ModelInstance>();

	directionalLight = std::make_unique<ModelInstance>();
	directionalLight->Init(GetMesh("DirectionalLight"));
	directionalLight->SetScale({ 1,1,1 });
	directionalLight->SetPosition(SimpleUtilities::Vector3f(0, 0, 0));
	directionalLight->SetName("DirectionalLight");

	return std::move(directionalLight);
}

std::unique_ptr<ModelInstance> ModelFactory::CreatePlaneModel()
{
	std::unique_ptr<ModelInstance> plane = std::make_unique<ModelInstance>();

	plane->Init(GetMesh("Plane"));
	plane->SetShader("Shaders/WaterReflectionPS.cso", "Shaders/DefaultVS.cso");
	plane->SetScale({ 1,1,1 });
	plane->SetPosition(SimpleUtilities::Vector3f(0, 0, 0));
	plane->SetName("Plane");

	return std::move(plane);
}

std::unique_ptr<ModelInstance> ModelFactory::CreateCubeModel()
{
	std::unique_ptr<ModelInstance> cube = std::make_unique<ModelInstance>();

	cube->Init(GetMesh("Cube"));
	cube->SetScale({ 1,1,1 });
	cube->SetPosition(SimpleUtilities::Vector3f(0, 0, 0));
	cube->SetName("Cube");

	return std::move(cube);
}

std::unique_ptr<ModelInstance> ModelFactory::CreatePyramidModel()
{
	std::unique_ptr<ModelInstance> pyramid = std::make_unique<ModelInstance>();

	pyramid->Init(GetMesh("Pyramid"), "Assets/Textures/Cat.dds");
	pyramid->SetScale({ 1,1,1 });
	pyramid->SetPosition(SimpleUtilities::Vector3f(0, 0, 0));
	pyramid->SetName("Pyramid");

	return std::move(pyramid);
}
