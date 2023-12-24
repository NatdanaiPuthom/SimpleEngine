#pragma once

class ModelInstance;
class Mesh;

class ModelFactory final
{
public:
	ModelFactory();
	~ModelFactory();

	void AddMesh(const char* aName, std::unique_ptr<Mesh> aMesh);
	Mesh* GetMesh(const char* aMeshName);

	std::unique_ptr<ModelInstance> CreateTerrainModel();
	std::unique_ptr<ModelInstance> CreateSkyBoxModel();
	std::unique_ptr<ModelInstance> CreateDirectionalLightModel();
	std::unique_ptr<ModelInstance> CreatePlaneModel();
	std::unique_ptr<ModelInstance> CreateCubeModel();
	std::unique_ptr<ModelInstance> CreatePyramidModel();
private:
	std::unordered_map<std::string, std::unique_ptr<Mesh>> myMeshes;
};
