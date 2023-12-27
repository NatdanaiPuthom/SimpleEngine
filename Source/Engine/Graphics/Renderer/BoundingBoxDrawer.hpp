#pragma once

class ConstantBuffer;
class ModelInstance;

class BoundingBoxDrawer
{
public:
	BoundingBoxDrawer();
	~BoundingBoxDrawer();

	bool Init(std::shared_ptr<ModelInstance> aModelInstance);
	void Render();

private:
	std::vector<Vertex> myVertices;
	std::vector<unsigned int> myIndices;

	Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> myIndexBuffer;

	std::unique_ptr<ConstantBuffer> myObjectBuffer;
	std::shared_ptr<Shader> myShader;

	SimpleUtilities::Matrix4x4f myMatrix;
};