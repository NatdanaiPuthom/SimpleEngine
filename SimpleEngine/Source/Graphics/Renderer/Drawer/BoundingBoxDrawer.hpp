#pragma once
#include "Graphics/Model/Mesh.hpp"

struct ID3D11Buffer;

namespace Graphics
{
	class Shader;
	class ConstantBuffer;
	class Model;
}

namespace Drawer
{
	class BoundingBoxDrawer final
	{
	public:
		BoundingBoxDrawer();
		~BoundingBoxDrawer();

		void Init();

		void Render(const std::shared_ptr<const Graphics::Model> aModelInstance);

	private:
		void InitMeshData2D();
		void InitMeshData3D();
		void InitObjectBuffer();
	private:
		Graphics::MeshData myMeshData3D;
		Graphics::MeshData myMeshData2D;

		ComPtr<ID3D11Buffer> myVertexBuffer;
		ComPtr<ID3D11Buffer> myIndexBuffer;

		std::unique_ptr<Graphics::ConstantBuffer> myTransformBuffer;
		std::shared_ptr<const Graphics::Shader> myShader3D;
		std::shared_ptr<const Graphics::Shader> myShader2D;
	};
}