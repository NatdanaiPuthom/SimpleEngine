#pragma once

class ConstantBuffer;
class Mesh;

class Renderer
{
public:
	Renderer();
	~Renderer();

	void AddMesh(std::unique_ptr<Mesh> aMesh);
	void Render();

	void InitTerrain();
	void InitDirectionalLight();
	void InitSkyBox();

	std::vector<Mesh*> GetMeshes();

private:
	std::vector<std::unique_ptr<Mesh>> myMeshes;

	std::unique_ptr<Mesh> myDirectionalLight;
};