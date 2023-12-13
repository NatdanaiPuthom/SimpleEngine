#include "stdafx.h"
#include "Engine/Graphics/Shapes/ShapeCreator3000.h"
#include <External/tga/Uppgift5/uppgift05_helper.h>

namespace SU = SimpleUtilities;

MeshData Shape::CreateTerrain()
{
	unsigned int gridSize = 12;
	unsigned int vertexSize = gridSize + 1;
	const unsigned int upSampleMultiply = 1;

	std::vector<float> heightMap(vertexSize * vertexSize);

	for (unsigned int i = 0; i < upSampleMultiply; i++)
	{
		heightMap = Tga::Upsample2X(heightMap, vertexSize);
		vertexSize *= 2;
	}

	Tga::AddNoise(heightMap, 0.5f);

	gridSize = vertexSize - 1;

	const float size = 0.2f;
	std::vector<Vertex> vertices(vertexSize * vertexSize);
	std::vector<unsigned int> indices;

	for (unsigned int y = 0; y < vertexSize; ++y)
	{
		for (unsigned int x = 0; x < vertexSize; ++x)
		{
			const unsigned int index = x + vertexSize * y;
			Vertex vertex;
			SU::Vector3f pos = SU::Vector3f(static_cast<float>(x), heightMap[index], static_cast<float>(y)) * size;
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
			SU::Vector3f normal = SU::Cross(tangentVertical, tangentHorizontal);
			normal.Normalize();
			vertices[index].normal = SU::Vector3f(normal.x, normal.y, normal.z);
			
			vertices[index].tangent = tangentHorizontal.GetNormalized();
			vertices[index].bitangent = tangentVertical.GetNormalized();
		}
	}

	return MeshData(vertices, indices);
}

MeshData Shape::CreatePyramid()
{
	//std::vector<Vertex> vertices =
	//{
	//	// Base
	//	{ SU::Vector4f(-1.0f, -1.0f, -1.0f, 1), SU::Vector4f(1.0f, 1.0f, 1.0f, 1.0f),SU::Vector4f(1, 1, 1, 1), SU::Vector2f(0.0f, 0.0f) },
	//	{ SU::Vector4f(1.0f, -1.0f, -1.0f, 1), SU::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(1.0f, 0.0f) },
	//	{ SU::Vector4f(1.0f, -1.0f, 1.0f, 1), SU::Vector4f(1.0f, 1.0f, 1.0f, 1.0f),SU::Vector4f(1, 1, 1, 1), SU::Vector2f(1.0f, 1.0f) },
	//	{ SU::Vector4f(-1.0f, -1.0f, 1.0f, 1), SU::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(0.0f, 1.0f) },

	//	// Top
	//	{ SU::Vector4f(0.0f, 1.0f, 0.0f, 1), SU::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(0.5f, 0.5f) }
	//};

	//std::vector<unsigned int> indices =
	//{
	//	// Base
	//	0, 1, 2,
	//	2, 3, 0,

	//	// Sides
	//	0, 4, 1,
	//	1, 4, 2,
	//	2, 4, 3,
	//	3, 4, 0
	//};

	//return MeshData(vertices, indices);
	return MeshData(std::vector<Vertex>(), std::vector<unsigned int>());
}

MeshData Shape::CreateCube()
{

	//std::vector<Vertex> vertices =
	//{
	//	//North
	//	Vertex{SU::Vector4f(0.5f, -0.5f, 0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(0, 1)}, //0
	//	{SU::Vector4f(0.5f, 0.5f, 0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(0, 0)}, //1
	//	{SU::Vector4f(-0.5f, 0.5f, 0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(1, 0)}, //2
	//	{SU::Vector4f(-0.5f, -0.5f, 0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector4f(1, 1, 1, 1), SU::Vector2f(1, 1)}, //3

	//	//West
	//	{SU::Vector4f(-0.5f, -0.5f, 0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector4f(1, 1, 1, 1), SU::Vector2f(0, 1)}, //4
	//	{SU::Vector4f(-0.5f, 0.5f, 0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(0, 0)}, //5
	//	{SU::Vector4f(-0.5f, 0.5f, -0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(1, 0)}, //6
	//	{SU::Vector4f(-0.5f, -0.5f, -0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector4f(1, 1, 1, 1), SU::Vector2f(1, 1)}, //7

	//	//South
	//	{SU::Vector4f(-0.5f, -0.5f, -0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector4f(1, 1, 1, 1), SU::Vector2f(0, 1)}, //8 A
	//	{SU::Vector4f(-0.5f, 0.5f, -0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector4f(1, 1, 1, 1), SU::Vector2f(0, 0)}, //9 B
	//	{SU::Vector4f(0.5f, 0.5f, -0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(1, 0)}, //10 C
	//	{SU::Vector4f(0.5f, -0.5f, -0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector4f(1, 1, 1, 1), SU::Vector2f(1, 1)}, //11 D

	//	//East
	//	{SU::Vector4f(0.5f, -0.5f, -0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector4f(1, 1, 1, 1), SU::Vector2f(0, 1)}, //12
	//	{SU::Vector4f(0.5f, 0.5f, -0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(0, 0)}, //13
	//	{SU::Vector4f(0.5f, 0.5f, 0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(1, 0)}, //14
	//	{SU::Vector4f(0.5f, -0.5f, 0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(1, 1)}, //15

	//	//Up
	//	{SU::Vector4f(0.5f, 0.5f, 0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(0, 1)}, //16
	//	{SU::Vector4f(0.5f, 0.5f, -0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(0, 0)}, //17
	//	{SU::Vector4f(-0.5f, 0.5f, -0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(1, 0)}, //18
	//	{SU::Vector4f(-0.5f, 0.5f, 0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector4f(1, 1, 1, 1), SU::Vector2f(1, 1)}, //19

	//	//Down
	//	{SU::Vector4f(-0.5f, -0.5f, 0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(0, 1)}, //20
	//	{SU::Vector4f(-0.5f, -0.5f, -0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector4f(1, 1, 1, 1), SU::Vector2f(0, 0)}, //21
	//	{SU::Vector4f(0.5f, -0.5f, -0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1),SU::Vector4f(1, 1, 1, 1), SU::Vector2f(1, 0)}, //22
	//	{SU::Vector4f(0.5f, -0.5f, 0.5f, 1.0f), SU::Vector4f(1, 1, 1, 1), SU::Vector4f(1, 1, 1, 1),SU::Vector2f(1, 1)} //23
	//};

	//std::vector<unsigned int> indices =
	//{
	//	0, 1, 2,
	//	0, 2, 3,
	//	4, 5, 6,
	//	4, 6, 7,
	//	8, 9, 10,
	//	8, 10, 11,
	//	12, 13, 14,
	//	12, 14, 15,
	//	16, 17, 18,
	//	16, 18, 19,
	//	20, 21, 22,
	//	20, 22, 23
	//};

	//return MeshData(vertices, indices);
	return MeshData(std::vector<Vertex>(), std::vector<unsigned int>());
}
