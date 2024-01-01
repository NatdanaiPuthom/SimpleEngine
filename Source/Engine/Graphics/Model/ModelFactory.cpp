#include "Engine/Precomplier/stdafx.h"
#include "Engine/Graphics/Shapes/ShapeCreator3000.hpp"
#include "Engine/NoClueWhatToName/SimpleGlobalImp.hpp"

ModelFactory::ModelFactory()
{
	SimpleGlobalModelFactoryImpl::SetModelFactory(this);

	MeshData cubeData = ShapeCreator3000::CreateCube();
	MeshData pyramidData = ShapeCreator3000::CreatePyramid();
	MeshData planeData = ShapeCreator3000::CreatePlane();
	MeshData skyboxData = ShapeCreator3000::CreateSkyBox(SimpleUtilities::Vector3f(100, 100, 100));
	MeshData terrainData = ShapeCreator3000::CreateTerrain();
	MeshData sphereData = ShapeCreator3000::CreateSphere();

	std::unique_ptr<Mesh> cube = std::make_unique<Mesh>();
	std::unique_ptr<Mesh> pyramid = std::make_unique<Mesh>();
	std::unique_ptr<Mesh> plane = std::make_unique<Mesh>();
	std::unique_ptr<Mesh> skyboxMesh = std::make_unique<Mesh>();
	std::unique_ptr<Mesh> directionalLight = std::make_unique<Mesh>();
	std::unique_ptr<Mesh> terrainMesh = std::make_unique<Mesh>();
	std::unique_ptr<Mesh> sphereMesh = std::make_unique<Mesh>();

	if (!cube->Init(cubeData))
		assert(false && "Failed to create Cube");

	if (!pyramid->Init(pyramidData))
		assert(false && "Failed to create Pyramid");

	if (!plane->Init(planeData))
		assert(false && "Failed to create Plane");

	if (!skyboxMesh->Init(skyboxData))
		assert(false && "Failed to create SkyBox");

	if (!terrainMesh->Init(terrainData))
		assert(false && "Failed to create Terrain");

	if (!sphereMesh->Init(sphereData))
		assert(false && "Failed to create Sphere");

	AddMesh("Cube", std::move(cube));
	AddMesh("Pyramid", std::move(pyramid));
	AddMesh("Plane", std::move(plane));
	AddMesh("Skybox", std::move(skyboxMesh));
	AddMesh("DirectionalLight", std::move(directionalLight));
	AddMesh("Terrain", std::move(terrainMesh));
	AddMesh("Sphere", std::move(sphereMesh));
}

ModelFactory::~ModelFactory()
{
}

void ModelFactory::AddMesh(const char* aName, std::unique_ptr<const Mesh> aMesh)
{
	myMeshes.emplace(aName, std::move(aMesh));
}

const Mesh* ModelFactory::GetMesh(const char* aMeshName)
{
	auto mesh = myMeshes.find(aMeshName);

	if (mesh != myMeshes.end())
		return mesh->second.get();

	return nullptr;
}

std::unique_ptr<Model> ModelFactory::CreateTerrainModel()
{
	std::unique_ptr<Model> terrainModel = std::make_unique<Model>();

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

std::unique_ptr<Model> ModelFactory::CreateSkyBoxModel()
{
	std::unique_ptr<Model> skyBoxModel = std::make_unique<Model>();

	skyBoxModel->Init(GetMesh("Skybox"), "Assets/tga/Uppgift7/cubemap.dds");
	skyBoxModel->SetShader("Shaders/SkyBoxPS.cso", "Shaders/SkyBoxVS.cso");
	skyBoxModel->SetScale({ 1,1,1 });
	skyBoxModel->SetPosition({ 0,0,0 });
	skyBoxModel->SetName("SkyBox");

	return std::move(skyBoxModel);
}

std::unique_ptr<Model> ModelFactory::CreateDirectionalLightModel()
{
	std::unique_ptr<Model> directionalLight = std::make_unique<Model>();

	directionalLight = std::make_unique<Model>();
	directionalLight->Init(GetMesh("DirectionalLight"));
	directionalLight->SetScale({ 1,1,1 });
	directionalLight->SetPosition(SimpleUtilities::Vector3f(0, 0, 0));
	directionalLight->SetName("DirectionalLight");

	return std::move(directionalLight);
}

std::unique_ptr<Model> ModelFactory::CreatePlaneModel()
{
	std::unique_ptr<Model> plane = std::make_unique<Model>();

	plane->Init(GetMesh("Plane"));
	plane->SetShader("Shaders/DefaultPS.cso", "Shaders/DefaultVS.cso");
	plane->SetScale({ 1,1,1 });
	plane->SetPosition(SimpleUtilities::Vector3f(0, 0, 0));
	plane->SetName("Plane");

	return std::move(plane);
}

std::unique_ptr<Model> ModelFactory::CreateCubeModel()
{
	std::unique_ptr<Model> cube = std::make_unique<Model>();

	cube->Init(GetMesh("Cube"));
	cube->SetScale({ 1,1,1 });
	cube->SetPosition(SimpleUtilities::Vector3f(0, 0, 0));
	cube->SetName("Cube");

	return std::move(cube);
}

std::unique_ptr<Model> ModelFactory::CreatePyramidModel()
{
	std::unique_ptr<Model> pyramid = std::make_unique<Model>();

	pyramid->Init(GetMesh("Pyramid"), "Assets/Textures/Cat.dds");
	pyramid->SetScale({ 1,1,1 });
	pyramid->SetPosition(SimpleUtilities::Vector3f(0, 0, 0));
	pyramid->SetName("Pyramid");

	return std::move(pyramid);
}

std::unique_ptr<Model> ModelFactory::CreateSphereModel()
{
	std::unique_ptr<Model> sphere = std::make_unique<Model>();

	sphere->Init(GetMesh("Sphere"));
	sphere->SetScale({ 1,1,1 });
	sphere->SetPosition(SimpleUtilities::Vector3f(0, 0, 0));
	sphere->SetName("Sphere");

	return std::move(sphere);
}

std::unique_ptr<Model> ModelFactory::CreatePlaneReflection()
{
	std::unique_ptr<Model> plane = std::make_unique<Model>();

	plane->Init(GetMesh("Plane"));
	plane->SetShader("Shaders/WaterReflectionPS.cso", "Shaders/PlaneReflectionVS.cso");
	plane->SetScale({ 1,1,1 });
	plane->SetPosition(SimpleUtilities::Vector3f(0, 0, 0));
	plane->SetName("PlaneReflection");

	return std::move(plane);
}