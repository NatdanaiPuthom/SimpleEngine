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
		const Graphics::Mesh* mesh = nullptr;
		const Graphics::Texture* texture = nullptr;
		const Graphics::Shader* shader = nullptr;
	};
}

#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace ECS
{
	REGISTER_COMPONENT(SkyBoxComponent);

	REGISTER_AND_EXPOSE_PROPERTY(&SkyBoxComponent::transform);
	REGISTER_AND_EXPOSE_PROPERTY(&SkyBoxComponent::mesh);
	REGISTER_AND_EXPOSE_PROPERTY(&SkyBoxComponent::texture);
	REGISTER_AND_EXPOSE_PROPERTY(&SkyBoxComponent::shader);
}