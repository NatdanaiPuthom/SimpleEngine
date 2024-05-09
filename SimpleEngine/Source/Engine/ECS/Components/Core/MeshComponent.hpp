#pragma once
#include "Graphics/Model/Mesh.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Texture/Texture.hpp"

namespace ECS
{
	struct MeshComponent final
	{
		const Graphics::Mesh* mesh;
		const Graphics::Shader* shader;
		const Graphics::Texture* texture;
	};
}