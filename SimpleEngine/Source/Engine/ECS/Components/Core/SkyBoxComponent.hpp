#pragma once
#include "Engine/ECS/Reflection/ECSReflection.hpp"
#include "Engine/Math/Transform.hpp"
#include "Graphics/Model/Mesh.hpp"
#include "Graphics/Texture/Texture.hpp"
#include "Graphics/Shaders/Shader.hpp"

namespace ECS
{
	COMPONENT(SkyBoxComponent) final
	{
		SkyBoxComponent();

		const Graphics::Mesh* mesh = nullptr;
		Math::Transform transform;
		const Graphics::Texture* texture = nullptr;
		const Graphics::Shader* shader = nullptr;
	};
}

namespace ECS
{
	//REGISTER_AND_EXPOSE_PROPERTY(&SkyBoxComponent::transform, false, false);
	REGISTER_AND_EXPOSE_PROPERTY(&SkyBoxComponent::mesh);
	REGISTER_AND_EXPOSE_PROPERTY(&SkyBoxComponent::texture);
	//REGISTER_AND_EXPOSE_PROPERTY(&SkyBoxComponent::shader);
}