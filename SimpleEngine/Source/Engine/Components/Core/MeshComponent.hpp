#pragma once
#include "Graphics/Model/Mesh.hpp"

namespace Graphics
{
	class Mesh;
	class Shader;
	class Texture;
}

namespace ECS
{
	struct MeshComponent
	{
		Graphics::Mesh mesh;
		const Graphics::Shader* shader;
		const Graphics::Texture* texture;
	};
}