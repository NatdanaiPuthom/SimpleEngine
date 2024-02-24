#pragma once

struct ID3D11Buffer;

namespace Simple
{
	struct MeshData;

	class Shader;
	class ConstantBuffer;
}

namespace Drawer
{
	struct Sphere
	{
		Math::Vector4f color = { 1.0f, 1.0f, 0.0f , 1.0f };
		Math::Vector3f position;
		float radius = 1.0f;
	};
}

namespace Drawer
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

		std::unique_ptr<Simple::MeshData> myMeshData;
		std::unique_ptr<Simple::ConstantBuffer> myObjectBuffer;
		std::shared_ptr<const  Simple::Shader> myShader;
	};
}