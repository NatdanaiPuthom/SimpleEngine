#pragma once

namespace Simple
{
	class Mesh;
	class Shader;
	class Texture;
}

struct MeshComponent
{
	const Simple::Mesh* mesh;
	const Simple::Shader* shader;
	const Simple::Texture* texture;
};