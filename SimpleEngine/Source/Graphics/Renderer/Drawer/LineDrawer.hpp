#pragma once
#include "Graphics/Model/Mesh.hpp"
#include <memory>
#include <vector>

struct ID3D11Buffer;

namespace Simple
{
	class Shader;
	class ConstantBuffer;
}

namespace Drawer
{
	struct Line final
	{
		Math::Vector4f color = { 0.0f, 1.0f, 0.0f, 1.0f };
		Math::Vector3f startPosition;
		Math::Vector3f endPosition;
	};
}

namespace Drawer
{
	class LineDrawer final
	{
		struct Data
		{
			Simple::MeshData meshData;
			ComPtr<ID3D11Buffer> vertexBuffer;
			ComPtr<ID3D11Buffer> indexBuffer;
		};

	public:
		LineDrawer();
		~LineDrawer();

		void Render(const Drawer::Line aLine);
		void RenderInstance(const std::vector<Drawer::Line>& aLines);
	public:
		size_t GetInstanceSizeLimit() const;
	private:
		void CreateBuffers();
		void CreateInstanceBuffer();
		void CreateObjectBuffer();
	private:
		std::unique_ptr<Data> myData;
		std::unique_ptr<Data> myInstanceData;

		std::unique_ptr<Simple::ConstantBuffer> myObjectBuffer;
		std::shared_ptr<const Simple::Shader> myShader;

		const size_t myInstanceSizeLimit;
	};
}