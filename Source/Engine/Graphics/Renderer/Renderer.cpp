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
	{ //Test Rotation Over Time
		//Pyramid
		SimpleUtilities::Vector3f pyramidRotation = myMeshes[1]->GetRotation();
		pyramidRotation.y += -10 * SimplyGlobal::GetDeltaTime();
		myMeshes[1]->SetRotation(pyramidRotation);

		//Cube
		SimpleUtilities::Vector3f cubeRotation = myMeshes[2]->GetRotation();
		cubeRotation.x += 10 * SimplyGlobal::GetDeltaTime();
		myMeshes[2]->SetRotation(cubeRotation);

		//Directional Light test
		SimpleUtilities::Vector3f directionalLight = SimplyGlobal::GetGraphicsEngine()->GetDirectionalLightDirection() * 180.0f;
		myMeshes[3]->SetRotation(directionalLight);
	}

	for (const auto& mesh : myMeshes)
	{
		mesh->Draw();
	}
}
