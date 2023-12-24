#pragma once

class Mesh;
class ModelInstance;

class Renderer final
{
public:
	Renderer();
	~Renderer();

	void Update();
	void Render();

	void AddMesh(const std::string& aName, std::unique_ptr<Mesh> aMesh);
	void AddModelInstance(std::unique_ptr<ModelInstance> aModelInstance);
public:
	Mesh* GetMesh(const std::string& aMeshName);
	std::vector<ModelInstance*> GetAllModelInstances();
private:
	void InitTerrain();
	void InitDirectionalLight();
	void InitSkyBox();
	void InitShapes();
private:
	std::unordered_map<std::string, std::unique_ptr<Mesh>> myMeshes;
	std::vector<std::unique_ptr<ModelInstance>> myModelInstances;
	std::unique_ptr<ModelInstance> myDirectionalLight;
};
