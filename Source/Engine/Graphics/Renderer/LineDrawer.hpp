#pragma once

struct ID3D11Buffer;
struct MeshData;

namespace Simple
{
	class Shader;
	class ConstantBuffer;
}

namespace Drawer
{
	struct Line final
	{
		SimpleUtilities::Vector4f color = { 0.0f, 1.0f, 0.0f, 1.0f };
		SimpleUtilities::Vector3f startPosition;
		SimpleUtilities::Vector3f endPosition;
	};
}

namespace Drawer
{
	class LineDrawer final
	{
	public:
		LineDrawer();
		~LineDrawer();

		void Render(const Drawer::Line& aLine);
	private:
		ComPtr<ID3D11Buffer> myVertexBuffer;
		ComPtr<ID3D11Buffer> myIndexBuffer;

		std::unique_ptr<MeshData> myMeshData;
		std::unique_ptr<Simple::ConstantBuffer> myObjectBuffer;
		std::shared_ptr<const Simple::Shader> myShader;
	};
}