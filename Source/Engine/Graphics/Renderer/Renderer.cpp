#include "stdafx.h"
#include "Engine/Graphics/Renderer/Renderer.hpp"

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
	{ //Test
		SimpleUtilities::Vector3f rotation = myMeshes[2]->GetRotation();
		rotation.y += 10 * SimplyGlobal::GetDeltaTime();
		myMeshes[2]->SetRotation(rotation);
	}

	for (const auto& mesh : myMeshes)
	{
		mesh->Draw();
	}
}
