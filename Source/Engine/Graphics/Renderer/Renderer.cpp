#include "stdafx.h"
#include "Engine/Graphics/Renderer/Renderer.hpp"
#include "Engine/Graphics/Shapes/ShapeCreator3000.hpp"

Renderer::Renderer()
	: mySkyBox(std::make_unique<Mesh>())
	, myDirectionalLight(std::make_unique<Mesh>())
{
	MeshData skyboxData = Shape::CreateSkyBox(SimpleUtilities::Vector3f(100, 100, 100));
	MeshData directionalLightData = Shape::CreateDirectionalLight();

	if (!mySkyBox->Init(skyboxData, "Shaders/SkyBoxPS.cso", "Shaders/SkyBoxVS.cso"))
		assert(false && "Failed to create SkyBox");

	if (!mySkyBox->AddTexture(14, "Assets/tga/Uppgift7/cubemap.dds"))
		assert(false && "Failed to Add Texture");

	if (!myDirectionalLight->Init(directionalLightData))
		assert(false && "Failed to create Directional Light");

	myDirectionalLight->SetPosition(SimpleUtilities::Vector3f(8, 6, 10));
	mySkyBox->SetPosition({ 0,0,20 });
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
		myDirectionalLight->SetRotation(directionalLight);
	}

	for (const auto& mesh : myMeshes)
	{
		mesh->Draw();
	}

	mySkyBox->Draw();
	myDirectionalLight->Draw();
}
