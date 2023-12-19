#include "stdafx.h"
#include "Engine/Graphics/Shapes/ShapeCreator3000.hpp"
#include <External/tga/Uppgift5/uppgift05_helper.h>

namespace SU = SimpleUtilities;

MeshData ShapeCreator3000::CreateTerrain(const Simple::TerrainData& aTerrainData)
{
	const unsigned int upSampleMultiplier = aTerrainData.upSampleMultiplier;
	const float vertexLength = aTerrainData.vertexLength;
	const float amplitude = aTerrainData.amplitude;

	unsigned int gridSize = aTerrainData.gridSize;

	std::vector<float> heightMap(gridSize * gridSize);

	{
		float amplitudeMultiplier = amplitude;
		for (unsigned int i = 0; i < upSampleMultiplier; ++i)
		{
			Tga::AddNoise(heightMap, amplitudeMultiplier);

			heightMap = Tga::Upsample2X(heightMap, gridSize);

			amplitudeMultiplier *= 0.25f;
			gridSize *= 2;
		}
	}

	std::vector<Vertex> vertices(gridSize * gridSize);
	std::vector<unsigned int> indices;

	for (unsigned int x = 0; x < gridSize; ++x)
	{
		for (unsigned int y = 0; y < gridSize; ++y)
		{
			const unsigned int index = x + (gridSize * y);

			const SU::Vector3f position = SU::Vector3f(static_cast<float>(x), heightMap[index], static_cast<float>(y)) * vertexLength;
			const float u = static_cast<float>(x) / gridSize;
			const float v = static_cast<float>(y) / gridSize;

			vertices[index].position = SU::Vector4f(position.x, position.y, position.z, 1.f);
			vertices[index].normal = SU::Vector3f(0, 1, 0);
			vertices[index].uv = SU::Vector2f(u, v);
		}
	}

	for (unsigned int x = 0; x < gridSize - 1; ++x)
	{
		for (unsigned int y = 0; y < gridSize - 1; ++y)
		{
			const unsigned int index = x + (gridSize * y);

			const unsigned int north = index + gridSize;
			const unsigned int northEast = north + 1;
			const unsigned int east = index + 1;

			// Clockwise - very important!!!
			indices.push_back(index);
			indices.push_back(north);
			indices.push_back(northEast);

			indices.push_back(index);
			indices.push_back(northEast);
			indices.push_back(east);
		}
	}

	for (unsigned int x = 0; x < gridSize; ++x)
	{
		for (unsigned int y = 0; y < gridSize; ++y)
		{
			const unsigned int index = x + (gridSize * y);

			unsigned int north = index + gridSize;
			unsigned int south = index - gridSize;
			unsigned int east = index + 1;
			unsigned int west = index - 1;

			if (y == 0)
			{
				south = index;
			}
			else if (y == (gridSize - 1))
			{
				north = index;
			}

			if (x == 0)
			{
				west = index;
			}
			else if (x == (gridSize - 1))
			{
				east = index;
			}

			SU::Vector3f tangentVertical = vertices[north].position.AsVector3() - vertices[south].position.AsVector3();
			SU::Vector3f tangentHorizontal = vertices[west].position.AsVector3() - vertices[east].position.AsVector3();
			SU::Vector3f normal = SU::Cross(tangentHorizontal, tangentVertical).GetNormalized();

			vertices[index].normal = normal;
			vertices[index].tangent = SU::Cross(vertices[index].normal, SU::Vector3f(0.0f, 0.0f, 1.0f)).GetNormalized();
			vertices[index].bitangent = SU::Cross(vertices[index].normal, vertices[index].tangent).GetNormalized();
		}
	}

	return MeshData(vertices, indices);
}

MeshData ShapeCreator3000::CreatePyramid(const SimpleUtilities::Vector3f aSize)
{
	Vertex south_West;
	south_West.position = SU::Vector4f(-aSize.x, -aSize.y, -aSize.z, 1);
	south_West.uv = SU::Vector2f(0.0f, 0.0f);

	Vertex south_East;
	south_East.position = SU::Vector4f(aSize.x, -aSize.y, -aSize.z, 1);
	south_East.uv = SU::Vector2f(1.0f, 0.0f);

	Vertex south_Top;
	south_Top.position = SU::Vector4f(0.0f, aSize.y, 0.0f, 1);
	south_Top.uv = SU::Vector2f(0.6f, 1.0f);

	Vertex east_West;
	east_West.position = SU::Vector4f(aSize.x, -aSize.y, -aSize.z, 1);
	east_West.uv = SU::Vector2f(0.0f, 0.0f);

	Vertex east_East;
	east_East.position = SU::Vector4f(aSize.x, -aSize.y, aSize.z, 1);
	east_East.uv = SU::Vector2f(1.0f, 0.0f);

	Vertex east_Top;
	east_Top.position = SU::Vector4f(0.0f, aSize.y, 0.0f, 1);
	east_Top.uv = SU::Vector2f(0.6f, 1.0f);

	Vertex north_West;
	north_West.position = SU::Vector4f(-aSize.x, -aSize.y, aSize.z, 1);
	north_West.uv = SU::Vector2f(0.0f, 0.0f);

	Vertex north_East;
	north_East.position = SU::Vector4f(aSize.x, -aSize.y, aSize.z, 1);
	north_East.uv = SU::Vector2f(1.0f, 0.0f);

	Vertex north_Top;
	north_Top.position = SU::Vector4f(0.0f, aSize.y, 0.0f, 1);
	north_Top.uv = SU::Vector2f(0.6f, 1.0f);

	Vertex west_West;
	west_West.position = SU::Vector4f(-aSize.x, -aSize.y, aSize.z, 1);
	west_West.uv = SU::Vector2f(0.0f, 0.0f);

	Vertex west_East;
	west_East.position = SU::Vector4f(-aSize.x, -aSize.y, -aSize.z, 1);
	west_East.uv = SU::Vector2f(1.0f, 0.0f);

	Vertex west_Top;
	west_Top.position = SU::Vector4f(0.0f, aSize.y, 0.0f, 1);
	west_Top.uv = SU::Vector2f(0.6f, 1.0f);

	std::vector<Vertex> vertices =
	{
		south_West, south_Top, south_East,
		east_West, east_Top, east_East,
		north_East, north_Top, north_West,
		west_West, west_Top, west_East
	};

	std::vector<unsigned int> indices =
	{
		0,1,2,
		3,4,5,
		6,7,8,
		9,10,11
	};

	return MeshData(vertices, indices);
}

MeshData ShapeCreator3000::CreateCube(const SimpleUtilities::Vector3f aSize)
{
	Vertex north_bottomLeft;
	north_bottomLeft.position = SU::Vector4f(aSize.x, -aSize.y, aSize.z, 1.0f);
	north_bottomLeft.uv = SU::Vector2f(0, 0);

	Vertex north_bottomRight;
	north_bottomRight.position = SU::Vector4f(-aSize.x, -aSize.y, aSize.z, 1.0f);
	north_bottomRight.uv = SU::Vector2f(1, 0);

	Vertex north_topLeft;
	north_topLeft.position = SU::Vector4f(aSize.x, aSize.y, aSize.z, 1.0f);
	north_topLeft.uv = SU::Vector2f(0, 1);

	Vertex north_topRight;
	north_topRight.position = SU::Vector4f(-aSize.x, aSize.y, aSize.z, 1.0f);
	north_topRight.uv = SU::Vector2f(1, 1);

	Vertex west_bottomLeft;
	west_bottomLeft.position = SU::Vector4f(-aSize.x, -aSize.y, aSize.z, 1.0f);
	west_bottomLeft.uv = SU::Vector2f(0, 0);

	Vertex west_bottomRight;
	west_bottomRight.position = SU::Vector4f(-aSize.x, -aSize.y, -aSize.z, 1.0f);
	west_bottomRight.uv = SU::Vector2f(1, 0);

	Vertex west_topLeft;
	west_topLeft.position = SU::Vector4f(-aSize.x, aSize.y, aSize.z, 1.0f);
	west_topLeft.uv = SU::Vector2f(0, 1);

	Vertex west_topRight;
	west_topRight.position = SU::Vector4f(-aSize.x, aSize.y, -aSize.z, 1.0f);
	west_topRight.uv = SU::Vector2f(1, 1);

	Vertex south_bottomLeft;
	south_bottomLeft.position = SU::Vector4f(-aSize.x, -aSize.y, -aSize.z, 1.0f);
	south_bottomLeft.uv = SU::Vector2f(0, 0);

	Vertex south_bottomRight;
	south_bottomRight.position = SU::Vector4f(aSize.x, -aSize.y, -aSize.z, 1.0f);
	south_bottomRight.uv = SU::Vector2f(1, 0);

	Vertex south_topLeft;
	south_topLeft.position = SU::Vector4f(-aSize.x, aSize.y, -aSize.z, 1.0f);
	south_topLeft.uv = SU::Vector2f(0, 1);

	Vertex south_topRight;
	south_topRight.position = SU::Vector4f(aSize.x, aSize.y, -aSize.z, 1.0f);
	south_topRight.uv = SU::Vector2f(1, 1);

	Vertex east_bottomLeft;
	east_bottomLeft.position = SU::Vector4f(aSize.x, -aSize.y, -aSize.z, 1.0f);
	east_bottomLeft.uv = SU::Vector2f(0, 0);

	Vertex east_bottomRight;
	east_bottomRight.position = SU::Vector4f(aSize.x, -aSize.y, aSize.z, 1.0f);
	east_bottomRight.uv = SU::Vector2f(1, 0);

	Vertex east_topLeft;
	east_topLeft.position = SU::Vector4f(aSize.x, aSize.y, -aSize.z, 1.0f);
	east_topLeft.uv = SU::Vector2f(0, 1);

	Vertex east_topRight;
	east_topRight.position = SU::Vector4f(aSize.x, aSize.y, aSize.z, 1.0f);
	east_topRight.uv = SU::Vector2f(1, 1);

	Vertex up_bottomLeft;
	up_bottomLeft.position = SU::Vector4f(aSize.x, aSize.y, aSize.z, 1.0f);
	up_bottomLeft.uv = SU::Vector2f(0, 0);

	Vertex up_bottomRight;
	up_bottomRight.position = SU::Vector4f(-aSize.x, aSize.y, aSize.z, 1.0f);
	up_bottomRight.uv = SU::Vector2f(1, 0);

	Vertex up_topLeft;
	up_topLeft.position = SU::Vector4f(aSize.x, aSize.y, -aSize.z, 1.0f);
	up_topLeft.uv = SU::Vector2f(0, 1);

	Vertex up_topRight;
	up_topRight.position = SU::Vector4f(-aSize.x, aSize.y, -aSize.z, 1.0f);
	up_topRight.uv = SU::Vector2f(1, 1);

	Vertex down_bottomLeft;
	down_bottomLeft.position = SU::Vector4f(-aSize.x, -aSize.y, aSize.z, 1.0f);
	down_bottomLeft.uv = SU::Vector2f(0, 0);

	Vertex down_bottomRight;
	down_bottomRight.position = SU::Vector4f(aSize.x, -aSize.y, aSize.z, 1.0f);
	down_bottomRight.uv = SU::Vector2f(1, 0);

	Vertex down_topLeft;
	down_topLeft.position = SU::Vector4f(-aSize.x, -aSize.y, -aSize.z, 1.0f);
	down_topLeft.uv = SU::Vector2f(0, 1);

	Vertex down_topRight;
	down_topRight.position = SU::Vector4f(aSize.x, -aSize.y, -aSize.z, 1.0f);
	down_topRight.uv = SU::Vector2f(1, 1);

	std::vector<Vertex> vertices =
	{
		north_bottomLeft, north_topLeft, north_topRight, north_bottomRight,
		west_bottomLeft, west_topLeft, west_topRight, west_bottomRight,
		south_bottomLeft, south_topLeft, south_topRight, south_bottomRight,
		east_bottomLeft, east_topLeft, east_topRight, east_bottomRight,
		up_bottomLeft, up_topLeft, up_topRight, up_bottomRight,
		down_bottomLeft, down_topLeft, down_topRight, down_bottomRight
	};

	std::vector<unsigned int> indices =
	{
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7,
		8, 9, 10,
		8, 10, 11,
		12, 13, 14,
		12, 14, 15,
		16, 17, 18,
		16, 18, 19,
		20, 21, 22,
		20, 22, 23
	};

	return MeshData(vertices, indices);
}

MeshData ShapeCreator3000::CreateDirectionalLight()
{
	Vertex north_bottomLeft;
	north_bottomLeft.color = { 1,0,0,1 };
	north_bottomLeft.position = SU::Vector4f(0.5f, -0.5f, 0.5f, 1.0f);

	Vertex north_bottomRight;
	north_bottomRight.color = { 1,0,0,1 };
	north_bottomRight.position = SU::Vector4f(-0.5f, -0.5f, 0.5f, 1.0f);

	Vertex north_topLeft;
	north_topLeft.color = { 1,0,0,1 };
	north_topLeft.position = SU::Vector4f(0.5f, 0.5f, 0.5f, 1.0f);

	Vertex north_topRight;
	north_topRight.color = { 1,0,0,1 };
	north_topRight.position = SU::Vector4f(-0.5f, 0.5f, 0.5f, 1.0f);

	Vertex west_bottomLeft;
	west_bottomLeft.color = { 0,1,0,1 };
	west_bottomLeft.position = SU::Vector4f(-0.5f, -0.5f, 0.5f, 1.0f);

	Vertex west_bottomRight;
	west_bottomRight.color = { 0,1,0,1 };
	west_bottomRight.position = SU::Vector4f(-0.5f, -0.5f, -0.5f, 1.0f);

	Vertex west_topLeft;
	west_topLeft.color = { 0,1,0,1 };
	west_topLeft.position = SU::Vector4f(-0.5f, 0.5f, 0.5f, 1.0f);

	Vertex west_topRight;
	west_topRight.color = { 0,1,0,1 };
	west_topRight.position = SU::Vector4f(-0.5f, 0.5f, -0.5f, 1.0f);

	Vertex south_bottomLeft;
	south_bottomLeft.color = { 0,0,1,1 };
	south_bottomLeft.position = SU::Vector4f(-0.5f, -0.5f, -0.5f, 1.0f);

	Vertex south_bottomRight;
	south_bottomRight.color = { 0,0,1,1 };
	south_bottomRight.position = SU::Vector4f(0.5f, -0.5f, -0.5f, 1.0f);

	Vertex south_topLeft;
	south_topLeft.color = { 0,0,1,1 };
	south_topLeft.position = SU::Vector4f(-0.5f, 0.5f, -0.5f, 1.0f);

	Vertex south_topRight;
	south_topRight.color = { 0,0,1,1 };
	south_topRight.position = SU::Vector4f(0.5f, 0.5f, -0.5f, 1.0f);

	Vertex east_bottomLeft;
	east_bottomLeft.color = { 0,1,1,1 };
	east_bottomLeft.position = SU::Vector4f(0.5f, -0.5f, -0.5f, 1.0f);

	Vertex east_bottomRight;
	east_bottomRight.color = { 0,1,1,1 };
	east_bottomRight.position = SU::Vector4f(0.5f, -0.5f, 0.5f, 1.0f);

	Vertex east_topLeft;
	east_topLeft.color = { 0,1,1,1 };
	east_topLeft.position = SU::Vector4f(0.5f, 0.5f, -0.5f, 1.0f);

	Vertex east_topRight;
	east_topRight.color = { 0,1,1,1 };
	east_topRight.position = SU::Vector4f(0.5f, 0.5f, 0.5f, 1.0f);

	Vertex up_bottomLeft;
	up_bottomLeft.color = { 1,1,1,1 };
	up_bottomLeft.position = SU::Vector4f(0.5f, 0.5f, 0.5f, 1.0f);

	Vertex up_bottomRight;
	up_bottomLeft.color = { 1,1,1,1 };
	up_bottomRight.position = SU::Vector4f(-0.5f, 0.5f, 0.5f, 1.0f);

	Vertex up_topLeft;
	up_topLeft.color = { 1,1,1,1 };
	up_topLeft.position = SU::Vector4f(0.5f, 0.5f, -0.5f, 1.0f);

	Vertex up_topRight;
	up_topLeft.color = { 1,1,1,1 };
	up_topRight.position = SU::Vector4f(-0.5f, 0.5f, -0.5f, 1.0f);

	Vertex down_bottomLeft;
	down_bottomLeft.color = { 0,0,0,1 };
	down_bottomLeft.position = SU::Vector4f(-0.5f, -0.5f, 0.5f, 1.0f);

	Vertex down_bottomRight;
	down_bottomRight.color = { 0,0,0,1 };
	down_bottomRight.position = SU::Vector4f(0.5f, -0.5f, 0.5f, 1.0f);

	Vertex down_topLeft;
	down_topLeft.color = { 0,0,0,1 };
	down_topLeft.position = SU::Vector4f(-0.5f, -0.5f, -0.5f, 1.0f);

	Vertex down_topRight;
	down_topRight.color = { 0,0,0,1 };
	down_topRight.position = SU::Vector4f(0.5f, -0.5f, -0.5f, 1.0f);

	std::vector<Vertex> vertices =
	{
		north_bottomLeft, north_topLeft, north_topRight, north_bottomRight,
		west_bottomLeft, west_topLeft, west_topRight, west_bottomRight,
		south_bottomLeft, south_topLeft, south_topRight, south_bottomRight,
		east_bottomLeft, east_topLeft, east_topRight, east_bottomRight,
		up_bottomLeft, up_topLeft, up_topRight, up_bottomRight,
		down_bottomLeft, down_topLeft, down_topRight, down_bottomRight
	};

	std::vector<unsigned int> indices =
	{
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7,
		8, 9, 10,
		8, 10, 11,
		12, 13, 14,
		12, 14, 15,
		16, 17, 18,
		16, 18, 19,
		20, 21, 22,
		20, 22, 23
	};

	return MeshData(vertices, indices);
}

MeshData ShapeCreator3000::CreateSkyBox(const SU::Vector3f& aSize)
{
	Vertex north_bottomLeft;
	north_bottomLeft.position = SU::Vector4f(aSize.x, -aSize.y, aSize.z, 1.0f);
	north_bottomLeft.uv = SU::Vector2f(0, 0);

	Vertex north_bottomRight;
	north_bottomRight.position = SU::Vector4f(-aSize.x, -aSize.y, aSize.z, 1.0f);
	north_bottomRight.uv = SU::Vector2f(1, 0);

	Vertex north_topLeft;
	north_topLeft.position = SU::Vector4f(aSize.x, aSize.y, aSize.z, 1.0f);
	north_topLeft.uv = SU::Vector2f(0, 1);

	Vertex north_topRight;
	north_topRight.position = SU::Vector4f(-aSize.x, aSize.y, aSize.z, 1.0f);
	north_topRight.uv = SU::Vector2f(1, 1);

	Vertex west_bottomLeft;
	west_bottomLeft.position = SU::Vector4f(-aSize.x, -aSize.y, aSize.z, 1.0f);
	west_bottomLeft.uv = SU::Vector2f(0, 0);

	Vertex west_bottomRight;
	west_bottomRight.position = SU::Vector4f(-aSize.x, -aSize.y, -aSize.z, 1.0f);
	west_bottomRight.uv = SU::Vector2f(1, 0);

	Vertex west_topLeft;
	west_topLeft.position = SU::Vector4f(-aSize.x, aSize.y, aSize.z, 1.0f);
	west_topLeft.uv = SU::Vector2f(0, 1);

	Vertex west_topRight;
	west_topRight.position = SU::Vector4f(-aSize.x, aSize.y, -aSize.z, 1.0f);
	west_topRight.uv = SU::Vector2f(1, 1);

	Vertex south_bottomLeft;
	south_bottomLeft.position = SU::Vector4f(-aSize.x, -aSize.y, -aSize.z, 1.0f);
	south_bottomLeft.uv = SU::Vector2f(0, 0);

	Vertex south_bottomRight;
	south_bottomRight.position = SU::Vector4f(aSize.x, -aSize.y, -aSize.z, 1.0f);
	south_bottomRight.uv = SU::Vector2f(1, 0);

	Vertex south_topLeft;
	south_topLeft.position = SU::Vector4f(-aSize.x, aSize.y, -aSize.z, 1.0f);
	south_topLeft.uv = SU::Vector2f(0, 1);

	Vertex south_topRight;
	south_topRight.position = SU::Vector4f(aSize.x, aSize.y, -aSize.z, 1.0f);
	south_topRight.uv = SU::Vector2f(1, 1);

	Vertex east_bottomLeft;
	east_bottomLeft.position = SU::Vector4f(aSize.x, -aSize.y, -aSize.z, 1.0f);
	east_bottomLeft.uv = SU::Vector2f(0, 0);

	Vertex east_bottomRight;
	east_bottomRight.position = SU::Vector4f(aSize.x, -aSize.y, aSize.z, 1.0f);
	east_bottomRight.uv = SU::Vector2f(1, 0);

	Vertex east_topLeft;
	east_topLeft.position = SU::Vector4f(aSize.x, aSize.y, -aSize.z, 1.0f);
	east_topLeft.uv = SU::Vector2f(0, 1);

	Vertex east_topRight;
	east_topRight.position = SU::Vector4f(aSize.x, aSize.y, aSize.z, 1.0f);
	east_topRight.uv = SU::Vector2f(1, 1);

	Vertex up_bottomLeft;
	up_bottomLeft.position = SU::Vector4f(aSize.x, aSize.y, aSize.z, 1.0f);
	up_bottomLeft.uv = SU::Vector2f(0, 0);

	Vertex up_bottomRight;
	up_bottomRight.position = SU::Vector4f(-aSize.x, aSize.y, aSize.z, 1.0f);
	up_bottomRight.uv = SU::Vector2f(1, 0);

	Vertex up_topLeft;
	up_topLeft.position = SU::Vector4f(aSize.x, aSize.y, -aSize.z, 1.0f);
	up_topLeft.uv = SU::Vector2f(0, 1);

	Vertex up_topRight;
	up_topRight.position = SU::Vector4f(-aSize.x, aSize.y, -aSize.z, 1.0f);
	up_topRight.uv = SU::Vector2f(1, 1);

	Vertex down_bottomLeft;
	down_bottomLeft.position = SU::Vector4f(-aSize.x, -aSize.y, aSize.z, 1.0f);
	down_bottomLeft.uv = SU::Vector2f(0, 0);

	Vertex down_bottomRight;
	down_bottomRight.position = SU::Vector4f(aSize.x, -aSize.y, aSize.z, 1.0f);
	down_bottomRight.uv = SU::Vector2f(1, 0);

	Vertex down_topLeft;
	down_topLeft.position = SU::Vector4f(-aSize.x, -aSize.y, -aSize.z, 1.0f);
	down_topLeft.uv = SU::Vector2f(0, 1);

	Vertex down_topRight;
	down_topRight.position = SU::Vector4f(aSize.x, -aSize.y, -aSize.z, 1.0f);
	down_topRight.uv = SU::Vector2f(1, 1);

	std::vector<Vertex> vertices =
	{
		north_bottomLeft, north_topLeft, north_topRight, north_bottomRight,
		west_bottomLeft, west_topLeft, west_topRight, west_bottomRight,
		south_bottomLeft, south_topLeft, south_topRight, south_bottomRight,
		east_bottomLeft, east_topLeft, east_topRight, east_bottomRight,
		up_bottomLeft, up_topLeft, up_topRight, up_bottomRight,
		down_bottomLeft, down_topLeft, down_topRight, down_bottomRight
	};

	std::vector<unsigned int> indices =
	{
		2, 1, 0,
		3, 2, 0,
		6, 5, 4,
		7, 6, 4,
		10, 9, 8,
		11, 10, 8,
		14, 13, 12,
		15, 14, 12,
		18, 17, 16,
		19, 18, 16,
		22, 21, 20,
		23, 22, 20
	};

	return MeshData(vertices, indices);
}
