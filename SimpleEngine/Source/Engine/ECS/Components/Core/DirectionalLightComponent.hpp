#pragma once
#include "Engine/Math/Transform.hpp"
#include "Graphics/Model/Mesh.hpp"
#include "Graphics/Texture/Texture.hpp"
#include "Graphics/Shaders/Shader.hpp"

namespace ECS
{
	struct DirectionalLightComponent final
	{
		DirectionalLightComponent();

		Math::Transform transform;
		const Graphics::Mesh* mesh;
		const Graphics::Texture* texture;
		const Graphics::Shader* shader;
	};
}

#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace ECS
{
	REGISTER_COMPONENT(DirectionalLightComponent);
	REGISTER_AND_EXPOSE_PROPERTY(&DirectionalLightComponent::transform);
	REGISTER_AND_EXPOSE_PROPERTY(&DirectionalLightComponent::mesh);
	REGISTER_AND_EXPOSE_PROPERTY(&DirectionalLightComponent::texture);
	REGISTER_AND_EXPOSE_PROPERTY(&DirectionalLightComponent::shader);
}