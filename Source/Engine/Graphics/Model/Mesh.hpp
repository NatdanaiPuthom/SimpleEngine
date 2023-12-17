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
	SimpleUtilities::Vector4f position = { 0,0,0,0 };
	SimpleUtilities::Vector4f color = { 1,1,1,1 };
	SimpleUtilities::Vector3f normal = { 0,0,0 };
	SimpleUtilities::Vector3f tangent = { 0,0,0 };
	SimpleUtilities::Vector3f bitangent = { 0,0,0 };
	SimpleUtilities::Vector2f uv = { 0,0 };
	float padding = -1;
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
	void SetRotation(const SimpleUtilities::Vector3f& aRotationInDegree);
	SimpleUtilities::Vector3f GetPosition() const;
	SimpleUtilities::Vector3f GetRotation() const;
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

	std::array<std::unique_ptr<Texture>, 15> myTextures;
};
