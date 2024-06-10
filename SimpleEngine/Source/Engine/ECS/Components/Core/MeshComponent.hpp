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

//namespace std
//{
//	REGISTER_DATATYPE_ARRAY_CONST_POINTER(array, int, 3);
//}

namespace Graphics
{
	REGISTER_DATATYPE_CONST_POINTER(Mesh);
}

namespace ECS
{
	REGISTER_COMPONENT(MeshComponent);

	EXPOSE_VARIABLE(&MeshComponent::mesh)
}