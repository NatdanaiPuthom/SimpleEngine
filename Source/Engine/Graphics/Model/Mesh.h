#pragma once
#include <wrl/client.h>

class GraphicsEngine;
class ConstantBuffer;
class Texture;
class Shader;

struct ID3D11Buffer;

using Microsoft::WRL::ComPtr;

struct alignas(16) Vertex
{
	SimpleUtilities::Vector4f position;
	SimpleUtilities::Vector4f color;
	SimpleUtilities::Vector3f normal;
	SimpleUtilities::Vector3f tangent;
	SimpleUtilities::Vector3f bitangent;
	SimpleUtilities::Vector2f uv;
	float padding;
};

struct MeshData
{
	std::vector<Vertex> myVertices;
	std::vector<unsigned int> myIndices;
};

class Mesh final
{
public:
	Mesh();
	~Mesh();

	const bool Init(const MeshData& aMeshData, const char* aPSShaderFile = "Shaders/DefaultPS.cso", const char* aVSShaderFile = "Shaders/DefaultVS.cso");
	void Draw();
public:
	void SetPosition(const SimpleUtilities::Vector3f& aPosition);
public:
	SimpleUtilities::Matrix4x4f& GetModelToWorldMatrix();
	ComPtr<ID3D11Buffer> GetVertexBuffer();
	ComPtr<ID3D11Buffer> GetIndexBuffer();
	Shader& GetShader();
	int GetIndexCount();
private:
	bool CreateVertexBuffer(ComPtr<ID3D11Device> aDevice);
	bool CreateIndexBuffer(ComPtr<ID3D11Device> aDevice);
	bool CreateObjectBuffer();
private:
	SimpleUtilities::Matrix4x4f myModelToWorld;

	MeshData myMeshData;
	GraphicsEngine* myGraphicsEngine;

	ComPtr<ID3D11Buffer> myVertexBuffer;
	ComPtr<ID3D11Buffer> myIndexBuffer;

	std::shared_ptr<Shader> myShader;
	std::unique_ptr<ConstantBuffer> myObjectBuffer;

	std::array<std::unique_ptr<Texture>, 6> myTextures;
};
