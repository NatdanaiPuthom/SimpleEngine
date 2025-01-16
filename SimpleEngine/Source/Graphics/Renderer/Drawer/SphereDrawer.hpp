#pragma once
#include "SimpleUtilities/Color.hpp"

struct ID3D11Buffer;

namespace Graphics
{
	struct MeshData;

	class Shader;
	class ConstantBuffer;
}

namespace Drawer
{
	struct Sphere
	{
		Simple::Color color = { 1.0f, 0.0f, 0.0f , 1.0f };
		Math::Vector3f position = { 0.0f, 0.0f,0.0f };
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

		std::unique_ptr<Graphics::MeshData> myMeshData;
		std::unique_ptr<Graphics::ConstantBuffer> myTransformBuffer;
		std::shared_ptr<const  Graphics::Shader> myShader;
	};
}