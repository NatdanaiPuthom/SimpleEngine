#pragma once

class GraphicsEngine;
class ConstantBuffer;
class Texture;

enum class eShaderType
{
	Default,
	Colorful
};

struct Vertex
{
	SimpleUtilities::Vector4f position;
	SimpleUtilities::Vector4f color;
	SimpleUtilities::Vector2f uv;
};

class Model
{
public:
	Model();
	~Model();

	virtual const bool Create() = 0;
	const bool Init(GraphicsEngine* aGraphicsEngine, const std::vector<Vertex>& aVertices, const std::vector<unsigned int>& aIndices, const SimpleUtilities::Matrix4x4f& aModelToWorld, eShaderType aShaderType = eShaderType::Default);
	void Draw();
public:
	SimpleUtilities::Matrix4x4f& GetModelToWorldMatrix();
	ComPtr<ID3D11Buffer> GetVertexBuffer();
	ComPtr<ID3D11Buffer> GetIndexBuffer();
	Shader& GetShader();
	int GetIndexCount();
protected:
	std::vector<Vertex> myVertices;
	std::vector<unsigned int> myIndices;

	SimpleUtilities::Matrix4x4f myModelToWorld;

	ComPtr<ID3D11Buffer> myVertexBuffer;
	ComPtr<ID3D11Buffer> myIndexBuffer;

	std::shared_ptr<Shader> myShader;
	std::unique_ptr<ConstantBuffer> myFrameBuffer;
	std::unique_ptr<ConstantBuffer> myObjectBuffer;
	std::unique_ptr<ConstantBuffer> myTimeBuffer;

	std::shared_ptr<Texture> myTexture;
private:
	GraphicsEngine* myGraphicsEngine;
};
