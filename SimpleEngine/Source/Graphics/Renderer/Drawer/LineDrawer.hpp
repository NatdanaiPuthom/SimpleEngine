#pragma once
#include "Graphics/Model/Mesh.hpp"
#include "SimpleUtilities/Color.hpp"
#include <memory>
#include <vector>

struct ID3D11Buffer;

namespace Graphics
{
	class Shader;
	class ConstantBuffer;
}

namespace Drawer
{
	struct Line final
	{
		Simple::Color color = { 0.0f, 1.0f, 0.0f, 1.0f };
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
			Graphics::MeshData meshData;
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
		void CreateTransformBuffer();
	private:
		std::unique_ptr<Data> myData;
		std::unique_ptr<Data> myInstanceData;

		std::unique_ptr<Graphics::ConstantBuffer> myTransformBuffer;
		std::shared_ptr<const Graphics::Shader> myShader;

		const size_t myInstanceSizeLimit;
	};
}