#pragma once

class ConstantBuffer;
class ModelInstance;

class BoundingBoxDrawer final
{
public:
	BoundingBoxDrawer();
	~BoundingBoxDrawer();

	void Render(const std::shared_ptr<const ModelInstance> aModelInstance);
public:
	void SetLineColor(const SimpleUtilities::Vector4f& aColor);
private:
	std::vector<Vertex> myVertices;
	std::vector<unsigned int> myIndices;

	Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> myIndexBuffer;

	std::unique_ptr<ConstantBuffer> myObjectBuffer;
	std::shared_ptr<Shader> myShader;

	SimpleUtilities::Vector4f myColor;
};