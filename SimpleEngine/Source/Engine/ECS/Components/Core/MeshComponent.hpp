#pragma once
#include "Engine/ECS/Reflection/ECSReflection.hpp"
#include "Graphics/Model/Mesh.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/Texture/Texture.hpp"
#include <array>

namespace ECS
{
	COMPONENT(MeshComponent) final
	{
		MeshComponent();

		std::array<const Graphics::Texture*, 3> textures;
		const Graphics::Mesh* mesh;
		const Graphics::Shader* shader;
	};
}

namespace ECS
{
	REGISTER_AND_EXPOSE_PROPERTY(&MeshComponent::mesh);
	REGISTER_AND_EXPOSE_PROPERTY(&MeshComponent::textures);
	//REGISTER_AND_EXPOSE_PROPERTY(&MeshComponent::shader);
}