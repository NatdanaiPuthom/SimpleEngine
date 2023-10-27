#pragma once

class GraphicsEngine;
class ConstantBuffer;

struct Vertex
{
	SimpleUtilities::Vector4f position;
	SimpleUtilities::Vector4f color;
};

class Model
{
public:
	Model();
	~Model();

	bool Init(GraphicsEngine* aGraphicsEngine, const std::vector<Vertex>& aVertices, const std::vector<unsigned int>& aIndices, const SimpleUtilities::Matrix4x4f& aModelToWorld, ID3D11Device* aDevice);
	void Draw(const float aDeltaTime);
public:
	SimpleUtilities::Matrix4x4f& GetModelToWorldMatrix();
	std::vector<D3D11_INPUT_ELEMENT_DESC> GetInputLayout();
	ComPtr<ID3D11Buffer> GetVertexBuffer();
	ComPtr<ID3D11Buffer> GetIndexBuffer();
	Shader& GetShader();
	int GetIndexCount();
private:
	std::vector<Vertex> myVertices;
	std::vector<unsigned int> myIndices;

	SimpleUtilities::Matrix4x4f myModelToWorld;

	ComPtr<ID3D11Buffer> myVertexBuffer;
	ComPtr<ID3D11Buffer> myIndexBuffer;

	std::shared_ptr<Shader> myShader;
	std::unique_ptr<ConstantBuffer> myFrameBuffer;
	std::unique_ptr<ConstantBuffer> myObjectBuffer;
	std::unique_ptr<ConstantBuffer> myTimeBuffer;
};
