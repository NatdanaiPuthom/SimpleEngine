#pragma once

class GraphicsEngine;
class ConstantBuffer;
class Texture;
class Shader;

struct ID3D11Buffer;
struct ID3D11ShaderResourceView;
struct ID3D11DeviceContext;

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

	const bool Init(const MeshData& aMeshData, const char* aPSShaderFile = "Shaders/DefaultPS.cso", const char* aVSShaderFile = "Shaders/DefaultVS.cso");
	const bool AddTexture(const char* aFilePath);

	void Draw();
public:
	void SetPosition(const SimpleUtilities::Vector3f& aPosition);
	void SetRotation(const SimpleUtilities::Vector3f& aRotationInDegree);
	void SetScale(const SimpleUtilities::Vector3f& aScale);
	void SetName(const std::string& aName);

	SimpleUtilities::Vector3f GetPosition() const;
	SimpleUtilities::Vector3f GetRotation() const;
	SimpleUtilities::Vector3f GetScale() const;
	std::string GetName() const;
public:
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	Shader& GetShader();
	int GetIndexCount();
private:
	bool CreateVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Device> aDevice);
	bool CreateIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Device> aDevice);
	bool CreateObjectBuffer();
private:
	std::vector<Texture*> myTextures;

	SimpleUtilities::Transform myTransform;

	MeshData myMeshData;
	std::string myName;

	GraphicsEngine* myGraphicsEngine;

	Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> myIndexBuffer;

	std::shared_ptr<Shader> myShader;
	std::unique_ptr<ConstantBuffer> myObjectBuffer;
};
