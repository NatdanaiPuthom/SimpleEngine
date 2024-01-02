#pragma once

namespace Simple
{
	class Model;
}

class ConstantBuffer;
class Shader;

struct ID3D11Buffer;
struct MeshData;

class BoundingBoxDrawer final
{
public:
	BoundingBoxDrawer();
	~BoundingBoxDrawer();

	void Render(const std::shared_ptr<const Simple::Model> aModelInstance);
private:
	MeshData myMeshData;

	Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> myIndexBuffer;

	std::unique_ptr<ConstantBuffer> myObjectBuffer;
	std::shared_ptr<const Shader> myShader;
};