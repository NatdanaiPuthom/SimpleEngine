#pragma once
#include "Engine/Math/Transform.hpp"
#include "Graphics/Model/Mesh.hpp"
#include "Graphics/Texture/Texture.hpp"
#include "Graphics/Shaders/Shader.hpp"

namespace ECS
{
	struct DirectionalLightComponent final
	{
		Math::Transform transform;
		const Graphics::Mesh* mesh = nullptr;
		const Graphics::Texture* texture = nullptr;
		const Graphics::Shader* shader = nullptr;
	};
}

#include "Engine/ECS/ECSReflection.hpp"

namespace ECS
{
	REGISTER_COMPONENT(DirectionalLightComponent);
}