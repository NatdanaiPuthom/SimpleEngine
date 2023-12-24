#pragma once

class ConstantBuffer;

struct ID3D11Buffer;
struct ID3D11Device;

struct Vertex
{
	SimpleUtilities::Vector4f position = { 0,0,0,0 };
	SimpleUtilities::Vector4f color = { 1,1,1,1 };
	SimpleUtilities::Vector3f normal = { 0,0,0 };
	SimpleUtilities::Vector3f tangent = { 0,0,0 };
	SimpleUtilities::Vector3f bitangent = { 0,0,0 };
	SimpleUtilities::Vector2f uv = { 0,0 };
	float clip = 0.0f;
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

	const bool Init(const MeshData& aMeshData);
	void BindMatrix(const SimpleUtilities::Matrix4x4f& aMatrix);
	void Draw();
private:
	bool CreateVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Device> aDevice);
	bool CreateIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Device> aDevice);
	bool CreateObjectBuffer();
private:
	MeshData myMeshData;
	Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> myIndexBuffer;
	std::unique_ptr<ConstantBuffer> myObjectBuffer;
};
