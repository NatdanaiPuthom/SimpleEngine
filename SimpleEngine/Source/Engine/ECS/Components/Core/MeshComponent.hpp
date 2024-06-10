#pragma once
#include "Graphics/Model/Mesh.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Texture/Texture.hpp"
#include <array>

namespace ECS
{
	struct MeshComponent final
	{
		std::array<const Graphics::Texture*, 3> textures;
		const Graphics::Mesh* mesh;
		const Graphics::Shader* shader;
	};
}

#include "Engine/ECS/Components/Core/MeshComponent.hpp"

namespace ECS
{
	REGISTER_COMPONENT(MeshComponent);
	EXPOSE_VARIABLE(&MeshComponent::mesh)
	EXPOSE_VARIABLE(&MeshComponent::textures)
	EXPOSE_VARIABLE(&MeshComponent::shader)
}