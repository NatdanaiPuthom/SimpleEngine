#pragma once

class Shader;

struct ID3D11Buffer;
struct MeshData;

namespace Simple
{
	class ConstantBuffer;
}

namespace Drawer
{
	struct Sphere
	{
		SimpleUtilities::Vector4f color = { 1.0f, 1.0f, 0.0f , 1.0f };
		SimpleUtilities::Vector3f position;
		float radius = 1.0f;
	};
}

namespace Simple
{
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
		std::unique_ptr<Simple::ConstantBuffer> myObjectBuffer;
		std::shared_ptr<const  Shader> myShader;
	};
}