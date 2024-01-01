#pragma once

class ConstantBuffer;
class Shader;

struct ID3D11Buffer;
struct MeshData;

namespace Drawer
{
	struct Sphere
	{
		SimpleUtilities::Vector4f color = { 1.0f, 1.0f, 0.0f , 1.0f };
		SimpleUtilities::Vector3f position;
		float radius = 1.0f;
	};
}

class SphereDrawer final
{
public:
	SphereDrawer();
	~SphereDrawer();

	void Render(const Drawer::Sphere& aSphere);
private:
	ComPtr<ID3D11Buffer> myVertexBuffer;
	ComPtr<ID3D11Buffer> myIndexBuffer;

	std::unique_ptr<MeshData> myMeshData;
	std::unique_ptr<ConstantBuffer> myObjectBuffer;
	std::shared_ptr<const  Shader> myShader;
};