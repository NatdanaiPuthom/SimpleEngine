#pragma once
#include "Engine/Math/Transform.hpp"
#include "Graphics/Model/Mesh.hpp"
#include "Graphics/Texture/Texture.hpp"
#include "Graphics/Shaders/Shader.hpp"

namespace ECS
{
	struct SkyBoxComponent final
	{
		Math::Transform transform;
		const Graphics::Mesh* mesh;
		const Graphics::Texture* texture;
		const Graphics::Shader* shader;
	};
}