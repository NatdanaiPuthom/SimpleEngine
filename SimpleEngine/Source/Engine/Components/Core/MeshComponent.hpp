#pragma once
#include "Graphics/Model/Mesh.hpp"

namespace Simple
{
	class Mesh;
	class Shader;
	class Texture;
}

namespace ECS
{
	struct MeshComponent
	{
		Simple::Mesh mesh;
		const Simple::Shader* shader;
		const Simple::Texture* texture;
	};
}