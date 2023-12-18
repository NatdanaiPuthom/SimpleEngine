#pragma once
#include "Engine/Graphics/Model/Mesh.hpp"

namespace Shape
{
	MeshData CreateTerrain();
	MeshData CreatePyramid();
	MeshData CreateCube(const SimpleUtilities::Vector3f aSize = {0.5f, 0.5f , 0.5f});
	MeshData CreateSkyBox(const SimpleUtilities::Vector3f& aSize = {100.0f, 100.0f, 100.0f});
	MeshData CreateDirectionalLight();
}
