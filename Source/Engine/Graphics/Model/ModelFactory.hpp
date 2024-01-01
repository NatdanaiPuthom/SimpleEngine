#pragma once

class ModelInstance;
class Mesh;

class ModelFactory final
{
public:
	ModelFactory();
	~ModelFactory();

	void AddMesh(const char* aName, std::unique_ptr<const Mesh> aMesh);
	const Mesh* GetMesh(const char* aMeshName);

public:
	std::unique_ptr<ModelInstance> CreateTerrainModel();
	std::unique_ptr<ModelInstance> CreateSkyBoxModel();
	std::unique_ptr<ModelInstance> CreateDirectionalLightModel();
	std::unique_ptr<ModelInstance> CreatePlaneModel();
	std::unique_ptr<ModelInstance> CreateCubeModel();
	std::unique_ptr<ModelInstance> CreatePyramidModel();
	std::unique_ptr<ModelInstance> CreateSphereModel();
	std::unique_ptr<ModelInstance> CreatePlaneReflection();
private:
	std::unordered_map<std::string, const std::unique_ptr<const Mesh>> myMeshes;
};