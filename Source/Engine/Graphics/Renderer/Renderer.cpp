#include "stdafx.h"
#include "Engine/Graphics/Renderer/Renderer.h"

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::AddMesh(std::unique_ptr<Mesh> aMesh)
{
	myMeshes.emplace_back(std::move(aMesh));
}

void Renderer::Render()
{
	for (const auto& mesh : myMeshes)
	{
		mesh->Draw();
	}
}
