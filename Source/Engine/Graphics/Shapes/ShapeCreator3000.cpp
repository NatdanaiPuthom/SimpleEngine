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
	Vertex southWest;
	southWest.position = SU::Vector4f(-1.0f, -1.0f, -1.0f, 1);
	southWest.uv = SU::Vector2f(0.0f, 0.0f);

	Vertex southEast;
	southEast.position = SU::Vector4f(1.0f, -1.0f, -1.0f, 1);
	southEast.uv = SU::Vector2f(1.0f, 0.0f);

	Vertex northEast;
	northEast.position = SU::Vector4f(1.0f, -1.0f, 1.0f, 1);
	northEast.uv = SU::Vector2f(1.0f, 1.0f);

	Vertex northWest;
	northWest.position = SU::Vector4f(-1.0f, -1.0f, 1.0f, 1);
	northWest.uv = SU::Vector2f(0.0f, 1.0f);

	Vertex top;
	top.position = SU::Vector4f(0.0f, 1.0f, 0.0f, 1);
	top.uv = SU::Vector2f(0.5f, 0.5f);

	std::vector<Vertex> vertices =
	{
		southWest, southEast, northEast, northWest, top
	};

	std::vector<unsigned int> indices =
	{
		// Base
		0, 1, 2,
		2, 3, 0,

		// Sides
		0, 4, 1,
		1, 4, 2,
		2, 4, 3,
		3, 4, 0
	};

	return MeshData(vertices, indices);
}

MeshData Shape::CreateCube()
{
	Vertex north1;
	north1.position = SU::Vector4f(0.5f, -0.5f, 0.5f, 1.0f);
	north1.uv = SU::Vector2f(0, 1);

	Vertex north2;
	north2.position = SU::Vector4f(0.5f, 0.5f, 0.5f, 1.0f);
	north2.uv = SU::Vector2f(0, 0);

	Vertex north3;
	north3.position = SU::Vector4f(-0.5f, 0.5f, 0.5f, 1.0f);
	north3.uv = SU::Vector2f(1, 0);

	Vertex north4;
	north4.position = SU::Vector4f(-0.5f, -0.5f, 0.5f, 1.0f);
	north4.uv = SU::Vector2f(1, 1);

	Vertex west1;
	west1.position = SU::Vector4f(-0.5f, -0.5f, 0.5f, 1.0f);
	west1.uv = SU::Vector2f(0, 1);

	Vertex west2;
	west2.position = SU::Vector4f(-0.5f, 0.5f, 0.5f, 1.0f);
	west2.uv = SU::Vector2f(0, 0);

	Vertex west3;
	west3.position = SU::Vector4f(-0.5f, 0.5f, -0.5f, 1.0f);
	west3.uv = SU::Vector2f(1, 0);

	Vertex west4;
	west4.position = SU::Vector4f(-0.5f, -0.5f, -0.5f, 1.0f);
	west4.uv = SU::Vector2f(1, 1);

	Vertex south1;
	south1.position = SU::Vector4f(-0.5f, -0.5f, -0.5f, 1.0f);
	south1.uv = SU::Vector2f(0, 1);

	Vertex south2;
	south2.position = SU::Vector4f(-0.5f, 0.5f, -0.5f, 1.0f);
	south2.uv = SU::Vector2f(0, 0);

	Vertex south3;
	south3.position = SU::Vector4f(0.5f, 0.5f, -0.5f, 1.0f);
	south3.uv = SU::Vector2f(1, 0);

	Vertex south4;
	south4.position = SU::Vector4f(0.5f, -0.5f, -0.5f, 1.0f);
	south4.uv = SU::Vector2f(1, 1);

	Vertex east1;
	east1.position = SU::Vector4f(0.5f, -0.5f, -0.5f, 1.0f);
	east1.uv = SU::Vector2f(0, 1);

	Vertex east2;
	east2.position = SU::Vector4f(0.5f, 0.5f, -0.5f, 1.0f);
	east2.uv = SU::Vector2f(0, 0);

	Vertex east3;
	east3.position = SU::Vector4f(0.5f, 0.5f, 0.5f, 1.0f);
	east3.uv = SU::Vector2f(1, 0);

	Vertex east4;
	east4.position = SU::Vector4f(0.5f, -0.5f, 0.5f, 1.0f);
	east4.uv = SU::Vector2f(1, 1);

	Vertex up1;
	up1.position = SU::Vector4f(0.5f, 0.5f, 0.5f, 1.0f);
	up1.uv = SU::Vector2f(0, 1);

	Vertex up2;
	up2.position = SU::Vector4f(0.5f, 0.5f, -0.5f, 1.0f);
	up2.uv = SU::Vector2f(0, 0);

	Vertex up3;
	up3.position = SU::Vector4f(-0.5f, 0.5f, -0.5f, 1.0f);
	up3.uv = SU::Vector2f(1, 0);

	Vertex up4;
	up4.position = SU::Vector4f(-0.5f, 0.5f, 0.5f, 1.0f);
	up4.uv = SU::Vector2f(1, 1);

	Vertex down1;
	down1.position = SU::Vector4f(-0.5f, -0.5f, 0.5f, 1.0f);
	down1.uv = SU::Vector2f(0, 1);

	Vertex down2;
	down2.position = SU::Vector4f(-0.5f, -0.5f, -0.5f, 1.0f);
	down2.uv = SU::Vector2f(0, 0);

	Vertex down3;
	down3.position = SU::Vector4f(0.5f, -0.5f, -0.5f, 1.0f);
	down3.uv = SU::Vector2f(1, 0);

	Vertex down4;
	down4.position = SU::Vector4f(0.5f, -0.5f, 0.5f, 1.0f);
	down4.uv = SU::Vector2f(1, 1);

	std::vector<Vertex> vertices =
	{
		north1, north2, north3, north4, 
		west1, west2, west3, west4, 
		south1, south2, south3, south4, 
		east1, east2, east3, east4,
		up1, up2, up3, up4,
		down1, down2, down3, down4
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
