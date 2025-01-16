#pragma once
#include "Engine/ECS/Reflection/ECSReflection.hpp"
#include "Engine/Math/Transform.hpp"
#include "Graphics/Model/Mesh.hpp"
#include "Graphics/Texture/Texture.hpp"
#include "Graphics/Shaders/Shader.hpp"

namespace ECS
{
	COMPONENT(DirectionalLightComponent) final
	{
		DirectionalLightComponent();

		Math::Transform transform;
		const Graphics::Mesh* mesh;
		const Graphics::Texture* texture;
		const Graphics::Shader* shader;
	};
}

namespace ECS
{
	REGISTER_AND_EXPOSE_PROPERTY(&DirectionalLightComponent::transform);
	REGISTER_AND_EXPOSE_PROPERTY(&DirectionalLightComponent::mesh);
	REGISTER_AND_EXPOSE_PROPERTY(&DirectionalLightComponent::texture);
	//REGISTER_AND_EXPOSE_PROPERTY(&DirectionalLightComponent::shader);
}