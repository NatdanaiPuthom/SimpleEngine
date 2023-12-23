#pragma once

class ConstantBuffer;
class Mesh;

class Renderer final
{
public:
	Renderer();
	~Renderer();

	void Update();
	void Render();

	void AddMesh(std::unique_ptr<Mesh> aMesh);
public:
	std::vector<Mesh*> GetMeshes();
private:
	void InitTerrain();
	void InitDirectionalLight();
	void InitSkyBox();
private:
	std::vector<std::unique_ptr<Mesh>> myMeshes;

	std::unique_ptr<Mesh> myDirectionalLight;
};