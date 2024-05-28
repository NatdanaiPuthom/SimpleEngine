#pragma once
#include "Engine/Math/Transform.hpp"
#include "Graphics/Model/Mesh.hpp"
#include "Graphics/Texture/Texture.hpp"
#include "Graphics/Shaders/Shader.hpp"

namespace ECS
{
	struct DirectionalLight final
	{
		Math::Transform transform;
		const Graphics::Mesh* mesh = nullptr;
		const Graphics::Texture* texture = nullptr;
		const Graphics::Shader* shader = nullptr;
	};
}