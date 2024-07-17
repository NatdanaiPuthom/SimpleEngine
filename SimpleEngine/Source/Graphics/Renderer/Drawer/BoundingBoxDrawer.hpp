#pragma once
#include "Engine/Math/Matrix4x4.hpp"
#include "Graphics/Model/Mesh.hpp"

struct ID3D11Buffer;

namespace Graphics
{
	class Shader;
	class ConstantBuffer;
	class Model;
}

namespace Simple
{
	struct BoundingBox3D;
}

namespace Drawer
{
	class BoundingBoxDrawer final
	{
	public:
		BoundingBoxDrawer();
		~BoundingBoxDrawer();

		void Init();
		void Render(const Simple::BoundingBox3D& aBoundingBox3D, const Math::Matrix4x4f& aModelToWorldMatrix);

	private:
		void InitMeshData2D();
		void InitMeshData3D();
		void InitObjectBuffer();
	private:
		Graphics::MeshData myMeshData3D;
		Graphics::MeshData myMeshData2D;

		ComPtr<ID3D11Buffer> myVertexBuffer;
		ComPtr<ID3D11Buffer> myIndexBuffer;

		std::shared_ptr<const Graphics::Shader> myShader3D;
		std::shared_ptr<const Graphics::Shader> myShader2D;

		std::unique_ptr<Graphics::ConstantBuffer> myTransformBuffer;
	};
}