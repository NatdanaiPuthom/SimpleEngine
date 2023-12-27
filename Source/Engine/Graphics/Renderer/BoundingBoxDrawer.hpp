#pragma once

class ConstantBuffer;
class ModelInstance;

class BoundingBoxDrawer
{
public:
	BoundingBoxDrawer();
	~BoundingBoxDrawer();

	void Render(const std::shared_ptr<const ModelInstance> aModelInstance);
private:
	std::vector<Vertex> myVertices;
	std::vector<unsigned int> myIndices;

	Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> myIndexBuffer;

	std::unique_ptr<ConstantBuffer> myObjectBuffer;
	std::shared_ptr<Shader> myShader;
};