#pragma once
#include "Graphics/Model/Mesh.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Texture/Texture.hpp"
#include <array>

namespace ECS
{
	struct MeshComponent final
	{
		MeshComponent();

		std::array<const Graphics::Texture*, 3> textures;
		const Graphics::Mesh* mesh;
		const Graphics::Shader* shader;
	};
}

#include "Engine/ECS/Reflection/ECSReflection.hpp"

namespace ECS
{
	REGISTER_COMPONENT(MeshComponent);
	REGISTER_AND_EXPOSE_PROPERTY(&MeshComponent::mesh);
	REGISTER_AND_EXPOSE_PROPERTY(&MeshComponent::textures);
	REGISTER_AND_EXPOSE_PROPERTY(&MeshComponent::shader);
}