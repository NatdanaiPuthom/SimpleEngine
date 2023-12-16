#include "stdafx.h"
#include "Engine/Graphics/Shapes/ShapeCreator3000.hpp"
#include <External/tga/Uppgift5/uppgift05_helper.h>

namespace SU = SimpleUtilities;

MeshData Shape::CreateTerrain()
{
	unsigned int gridSize = 12;
	unsigned int vertexSize = gridSize + 1;

	const unsigned int upSampleMultiply = 2;

	const float vertexDistance = 0.50f;
	const float amplitude = 5.f;

	std::vector<float> heightMap(vertexSize * vertexSize);

	float amplitudeMultiplier = amplitude;
	for (unsigned int i = 0; i < upSampleMultiply; i++)
	{
		Tga::AddNoise(heightMap, amplitudeMultiplier);
		heightMap = Tga::Upsample2X(heightMap, vertexSize);
		amplitudeMultiplier *= 0.25f;
		vertexSize *= 2;
	}

	gridSize = vertexSize - 1;

	std::vector<Vertex> vertices(vertexSize * vertexSize);
	std::vector<unsigned int> indices;

	for (unsigned int y = 0; y < vertexSize; ++y)
	{
		for (unsigned int x = 0; x < vertexSize; ++x)
		{
			const unsigned int index = x + vertexSize * y;
			Vertex vertex;
			SU::Vector3f pos = SU::Vector3f(static_cast<float>(x), heightMap[index], static_cast<float>(y)) * vertexDistance;
			vertex.position = SU::Vector4f(pos.x, pos.y, pos.z, 1.f);
			vertex.color = SU::Vector4f(1.f, 1.f, 1.f, 1.f);
			vertex.uv = { static_cast<float>(x) / vertexSize, static_cast<float>(y) / vertexSize };
			vertex.normal = SU::Vector3f(0, 1, 0);
			vertices[index] = vertex;
		}
	}

	for (unsigned int y = 0; y < gridSize; ++y)
	{
		for (unsigned int x = 0; x < gridSize; ++x)
		{
			const unsigned int index = x * vertexSize + y;

			const unsigned int indexUp = index + vertexSize;
			const unsigned int indexDiagonal = indexUp + 1;
			const unsigned int indexRight = index + 1;

			// Clockwise - very important!!!
			indices.push_back(index);
			indices.push_back(indexUp);
			indices.push_back(indexDiagonal);

			indices.push_back(index);
			indices.push_back(indexDiagonal);
			indices.push_back(indexRight);
		}
	}

	for (unsigned int y = 1; y < gridSize; ++y)
	{
		for (unsigned int x = 1; x < gridSize; ++x)
		{
			const unsigned int index = x * vertexSize + y;

			const unsigned int indexUp = index + vertexSize;
			const unsigned int indexDown = index - vertexSize;
			const unsigned int indexRight = index + 1;
			const unsigned int indexLeft = index - 1;

			SU::Vector3f tangentVertical = vertices[indexUp].position.AsVector3() - vertices[indexDown].position.AsVector3();
			SU::Vector3f tangentHorizontal = vertices[indexRight].position.AsVector3() - vertices[indexLeft].position.AsVector3();
			SU::Vector3f normal = SU::Cross(tangentVertical, tangentHorizontal).GetNormalized();

			vertices[index].normal = SU::Vector3f(normal.x, normal.y, normal.z).GetNormalized();
			vertices[index].tangent = SU::Cross(vertices[index].normal, SU::Vector3f(0.0f, 0.0f, 1.0f)).GetNormalized();
			vertices[index].bitangent = SU::Cross(vertices[index].normal, vertices[index].tangent).GetNormalized();
		}
	}

	return MeshData(vertices, indices);
}

MeshData Shape::CreatePyramid()
{
	Vertex south_West;
	south_West.position = SU::Vector4f(-1.0f, -1.0f, -1.0f, 1);
	south_West.uv = SU::Vector2f(0.0f, 0.0f);

	Vertex south_East;
	south_East.position = SU::Vector4f(1.0f, -1.0f, -1.0f, 1);
	south_East.uv = SU::Vector2f(1.0f, 0.0f);

	Vertex south_Top;
	south_Top.position = SU::Vector4f(0.0f, 1.0f, 0.0f, 1);
	south_Top.uv = SU::Vector2f(0.0f, 1.0f);

	Vertex east_West;
	east_West.position = SU::Vector4f(1.0f, -1.0f, -1.0f, 1);
	east_West.uv = SU::Vector2f(0.0f, 0.0f);

	Vertex east_East;
	east_East.position = SU::Vector4f(1.0f, -1.0f, 1.0f, 1);
	east_East.uv = SU::Vector2f(1.0f, 0.0f);

	Vertex east_Top;
	east_Top.position = SU::Vector4f(0.0f, 1.0f, 0.0f, 1);
	east_Top.uv = SU::Vector2f(0.0f, 1.0f);

	Vertex north_West;
	north_West.position = SU::Vector4f(-1.0f, -1.0f, 1.0f, 1);
	north_West.uv = SU::Vector2f(0.0f, 0.0f);

	Vertex north_East;
	north_East.position = SU::Vector4f(1.0f, -1.0f, 1.0f, 1);
	north_East.uv = SU::Vector2f(1.0f, 0.0f);

	Vertex north_Top;
	north_Top.position = SU::Vector4f(0.0f, 1.0f, 0.0f, 1);
	north_Top.uv = SU::Vector2f(0.0f, 1.0f);

	Vertex west_West;
	west_West.position = SU::Vector4f(-1.0f, -1.0f, 1.0f, 1);
	west_West.uv = SU::Vector2f(0.0f, 0.0f);

	Vertex west_East;
	west_East.position = SU::Vector4f(-1.0f, -1.0f, -1.0f, 1);
	west_East.uv = SU::Vector2f(1.0f, 0.0f);

	Vertex west_Top;
	west_Top.position = SU::Vector4f(0.0f, 1.0f, 0.0f, 1);
	west_Top.uv = SU::Vector2f(0.0f, 1.0f);

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

MeshData Shape::CreateCube()
{
	Vertex north_bottomLeft;
	north_bottomLeft.position = SU::Vector4f(0.5f, -0.5f, 0.5f, 1.0f);
	north_bottomLeft.uv = SU::Vector2f(0, 0);

	Vertex north_bottomRight;
	north_bottomRight.position = SU::Vector4f(-0.5f, -0.5f, 0.5f, 1.0f);
	north_bottomRight.uv = SU::Vector2f(1, 0);

	Vertex north_topLeft;
	north_topLeft.position = SU::Vector4f(0.5f, 0.5f, 0.5f, 1.0f);
	north_topLeft.uv = SU::Vector2f(0, 1);

	Vertex north_topRight;
	north_topRight.position = SU::Vector4f(-0.5f, 0.5f, 0.5f, 1.0f);
	north_topRight.uv = SU::Vector2f(1, 1);

	Vertex west_bottomLeft;
	west_bottomLeft.position = SU::Vector4f(-0.5f, -0.5f, 0.5f, 1.0f);
	west_bottomLeft.uv = SU::Vector2f(0, 0);

	Vertex west_bottomRight;
	west_bottomRight.position = SU::Vector4f(-0.5f, -0.5f, -0.5f, 1.0f);
	west_bottomRight.uv = SU::Vector2f(1, 0);

	Vertex west_topLeft;
	west_topLeft.position = SU::Vector4f(-0.5f, 0.5f, 0.5f, 1.0f);
	west_topLeft.uv = SU::Vector2f(0, 1);

	Vertex west_topRight;
	west_topRight.position = SU::Vector4f(-0.5f, 0.5f, -0.5f, 1.0f);
	west_topRight.uv = SU::Vector2f(1, 1);

	Vertex south_bottomLeft;
	south_bottomLeft.position = SU::Vector4f(-0.5f, -0.5f, -0.5f, 1.0f);
	south_bottomLeft.uv = SU::Vector2f(0, 0);

	Vertex south_bottomRight;
	south_bottomRight.position = SU::Vector4f(0.5f, -0.5f, -0.5f, 1.0f);
	south_bottomRight.uv = SU::Vector2f(1, 0);

	Vertex south_topLeft;
	south_topLeft.position = SU::Vector4f(-0.5f, 0.5f, -0.5f, 1.0f);
	south_topLeft.uv = SU::Vector2f(0, 1);

	Vertex south_topRight;
	south_topRight.position = SU::Vector4f(0.5f, 0.5f, -0.5f, 1.0f);
	south_topRight.uv = SU::Vector2f(1, 1);

	Vertex east_bottomLeft;
	east_bottomLeft.position = SU::Vector4f(0.5f, -0.5f, -0.5f, 1.0f);
	east_bottomLeft.uv = SU::Vector2f(0, 0);

	Vertex east_bottomRight;
	east_bottomRight.position = SU::Vector4f(0.5f, -0.5f, 0.5f, 1.0f);
	east_bottomRight.uv = SU::Vector2f(1, 0);

	Vertex east_topLeft;
	east_topLeft.position = SU::Vector4f(0.5f, 0.5f, -0.5f, 1.0f);
	east_topLeft.uv = SU::Vector2f(0, 1);

	Vertex east_topRight;
	east_topRight.position = SU::Vector4f(0.5f, 0.5f, 0.5f, 1.0f);
	east_topRight.uv = SU::Vector2f(1, 1);

	Vertex up_bottomLeft;
	up_bottomLeft.position = SU::Vector4f(0.5f, 0.5f, 0.5f, 1.0f);
	up_bottomLeft.uv = SU::Vector2f(0, 0);

	Vertex up_bottomRight;
	up_bottomRight.position = SU::Vector4f(-0.5f, 0.5f, 0.5f, 1.0f);
	up_bottomRight.uv = SU::Vector2f(1, 0);

	Vertex up_topLeft;
	up_topLeft.position = SU::Vector4f(0.5f, 0.5f, -0.5f, 1.0f);
	up_topLeft.uv = SU::Vector2f(0, 1);

	Vertex up_topRight;
	up_topRight.position = SU::Vector4f(-0.5f, 0.5f, -0.5f, 1.0f);
	up_topRight.uv = SU::Vector2f(1, 1);

	Vertex down_bottomLeft;
	down_bottomLeft.position = SU::Vector4f(-0.5f, -0.5f, 0.5f, 1.0f);
	down_bottomLeft.uv = SU::Vector2f(0, 0);

	Vertex down_bottomRight;
	down_bottomRight.position = SU::Vector4f(0.5f, -0.5f, 0.5f, 1.0f);
	down_bottomRight.uv = SU::Vector2f(1, 0);

	Vertex down_topLeft;
	down_topLeft.position = SU::Vector4f(-0.5f, -0.5f, -0.5f, 1.0f);
	down_topLeft.uv = SU::Vector2f(0, 1);

	Vertex down_topRight;
	down_topRight.position = SU::Vector4f(0.5f, -0.5f, -0.5f, 1.0f);
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
