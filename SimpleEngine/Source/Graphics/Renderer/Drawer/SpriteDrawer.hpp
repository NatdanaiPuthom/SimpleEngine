#pragma once
#include "Engine/Math/Vector2.hpp"
#include <memory>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

struct ID3D11Buffer;

namespace Graphics
{
	struct MeshData;

	class Shader;
	class ConstantBuffer;
	class Texture;
}

namespace Drawer
{
	struct Sprite2D
	{
		Math::Vector2f position;
		Math::Vector2f size;
		std::shared_ptr<const Graphics::Texture> texture;
	};
}

namespace Drawer
{
	class SpriteDrawer final
	{
	public:
		SpriteDrawer();
		~SpriteDrawer();

		void Render(const Sprite2D& aSprite);
	private:
		std::shared_ptr<const Graphics::Shader> myShader;

		std::unique_ptr<Graphics::ConstantBuffer> myTransformBuffer;
		std::unique_ptr<const Graphics::MeshData> myMeshData;

		ComPtr<ID3D11Buffer> myVertexBuffer;
		ComPtr<ID3D11Buffer> myIndexBuffer;
	};
}