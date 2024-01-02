#pragma once


struct ID3D11Buffer;
struct MeshData;

namespace Simple
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

		void Render(const std::shared_ptr<const Simple::Model> aModelInstance);
	private:
		MeshData myMeshData;

		Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> myIndexBuffer;

		std::unique_ptr<Simple::ConstantBuffer> myObjectBuffer;
		std::shared_ptr<const Simple::Shader> myShader;
	};
}