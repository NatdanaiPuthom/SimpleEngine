#pragma once

class Model;
class Mesh;

class ModelFactory final
{
public:
	ModelFactory();
	~ModelFactory();

	void AddMesh(const char* aName, std::unique_ptr<const Mesh> aMesh);
	const Mesh* GetMesh(const char* aMeshName);

public:
	std::unique_ptr<Model> CreateTerrainModel();
	std::unique_ptr<Model> CreateSkyBoxModel();
	std::unique_ptr<Model> CreateDirectionalLightModel();
	std::unique_ptr<Model> CreatePlaneModel();
	std::unique_ptr<Model> CreateCubeModel();
	std::unique_ptr<Model> CreatePyramidModel();
	std::unique_ptr<Model> CreateSphereModel();
	std::unique_ptr<Model> CreatePlaneReflection();
private:
	std::unordered_map<std::string, const std::unique_ptr<const Mesh>> myMeshes;
};