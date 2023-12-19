#pragma once

class GraphicsEngine;
class ConstantBuffer;
class Texture;
class Shader;

struct ID3D11Buffer;

struct Vertex
{
	SimpleUtilities::Vector4f position = { 0,0,0,0 };
	SimpleUtilities::Vector4f color = { 1,1,1,1 };
	SimpleUtilities::Vector3f normal = { 0,0,0 };
	SimpleUtilities::Vector3f tangent = { 0,0,0 };
	SimpleUtilities::Vector3f bitangent = { 0,0,0 };
	SimpleUtilities::Vector2f uv = { 0,0 };
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
	const bool AddTexture(const int aSlot, const char* aFilePath);

	void Draw();
public:
	void SetPosition(const SimpleUtilities::Vector3f& aPosition);
	void SetRotation(const SimpleUtilities::Vector3f& aRotationInDegree);
	SimpleUtilities::Vector3f GetPosition() const;
	SimpleUtilities::Vector3f GetRotation() const;
public:
	SimpleUtilities::Matrix4x4f GetModelToWorldMatrix();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	Shader& GetShader();
	int GetIndexCount();
private:
	bool CreateVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Device> aDevice);
	bool CreateIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Device> aDevice);
	bool CreateObjectBuffer();
private:
	SimpleUtilities::Matrix4x4f myModelToWorld;

	MeshData myMeshData;
	GraphicsEngine* myGraphicsEngine;

	Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> myIndexBuffer;

	std::shared_ptr<Shader> myShader;
	std::unique_ptr<ConstantBuffer> myObjectBuffer;

	std::array<std::unique_ptr<Texture>, 15> myTextures;
};
