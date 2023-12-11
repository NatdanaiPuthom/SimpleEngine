#pragma once
#include <wrl/client.h>

class GraphicsEngine;
class ConstantBuffer;
class Texture;
class Shader;

using Microsoft::WRL::ComPtr;
struct ID3D11Buffer;

struct Vertex
{
	SimpleUtilities::Vector4f position;
	SimpleUtilities::Vector4f color;
	SimpleUtilities::Vector4f normal;
	SimpleUtilities::Vector2f uv;
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

	const bool Init(const MeshData& aMeshData, const char* aPSShaderFile = "DefaultPS.cso", const char* aVSShaderFile = "DefaultVS.cso", const char* aTextureFile = "colors.dds");
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
	GraphicsEngine* myGraphicsEngine;

	SimpleUtilities::Matrix4x4f myModelToWorld;

	MeshData myMeshData;

	ComPtr<ID3D11Buffer> myVertexBuffer;
	ComPtr<ID3D11Buffer> myIndexBuffer;

	std::shared_ptr<Shader> myShader;

	std::unique_ptr<Texture> myTexture;
	std::unique_ptr<ConstantBuffer> myFrameBuffer;
	std::unique_ptr<ConstantBuffer> myObjectBuffer;
	std::unique_ptr<ConstantBuffer> myTimeBuffer;
	std::unique_ptr<ConstantBuffer> myDirectionLightBuffer;
};
