#pragma once
#include "Engine/Graphics/Model/Mesh.hpp"

namespace Shape
{
	MeshData CreateTerrain();
	MeshData CreatePyramid();
	MeshData CreateCube();
	MeshData CreateDirectionalLight();
	MeshData CreateSkyBox(const SimpleUtilities::Vector3f& aSize);
}