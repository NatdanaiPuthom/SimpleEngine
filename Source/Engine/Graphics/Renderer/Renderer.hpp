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

private:
	std::vector<std::unique_ptr<Mesh>> myMeshes;
};