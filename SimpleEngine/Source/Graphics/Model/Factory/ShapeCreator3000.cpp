#include "Graphics/Precomplied/GraphicsPch.hpp"
#include "Graphics/Model/Factory/ShapeCreator3000.hpp"
#include <External/TheGameAssembly/Uppgift5/uppgift05_helper.h>

namespace SU = SimpleUtilities;

namespace Simple
{
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

				const Math::Vector3f position = Math::Vector3f(static_cast<float>(x), heightMap[index], static_cast<float>(y)) * vertexLength;
				const float u = static_cast<float>(x) / gridSize;
				const float v = static_cast<float>(y) / gridSize;

				vertices[index].position = Math::Vector4f(position.x, position.y, position.z, 1.f);
				vertices[index].normal = Math::Vector3f(0, 1, 0);
				vertices[index].uv = Math::Vector2f(u, v);
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

				Math::Vector3f tangentVertical = vertices[north].position.AsVector3() - vertices[south].position.AsVector3();
				Math::Vector3f tangentHorizontal = vertices[west].position.AsVector3() - vertices[east].position.AsVector3();
				Math::Vector3f normal = Math::Cross(tangentHorizontal, tangentVertical).GetNormalized();

				vertices[index].normal = normal;
				vertices[index].tangent = Math::Cross(vertices[index].normal, Math::Vector3f(0.0f, 0.0f, 1.0f)).GetNormalized();
				vertices[index].bitangent = Math::Cross(vertices[index].normal, vertices[index].tangent).GetNormalized();
			}
		}

		return MeshData(vertices, indices);
	}

	MeshData ShapeCreator3000::CreatePyramid(const Math::Vector3f& aSize)
	{
		Math::Vector3f normal;

		Vertex south_West;
		south_West.position = Math::Vector4f(-aSize.x, -aSize.y, -aSize.z, 1);
		south_West.uv = Math::Vector2f(0.0f, 0.0f);

		Vertex south_East;
		south_East.position = Math::Vector4f(aSize.x, -aSize.y, -aSize.z, 1);
		south_East.uv = Math::Vector2f(1.0f, 0.0f);

		Vertex south_Top;
		south_Top.position = Math::Vector4f(0.0f, aSize.y, 0.0f, 1);
		south_Top.uv = Math::Vector2f(0.6f, 1.0f);

		normal = CalcualtePyramidNormal(south_West.position.AsVector3(), south_Top.position.AsVector3(), south_East.position.AsVector3());
		south_West.normal = normal;
		south_East.normal = normal;
		south_Top.normal = normal;

		Vertex east_West;
		east_West.position = Math::Vector4f(aSize.x, -aSize.y, -aSize.z, 1);
		east_West.uv = Math::Vector2f(0.0f, 0.0f);

		Vertex east_East;
		east_East.position = Math::Vector4f(aSize.x, -aSize.y, aSize.z, 1);
		east_East.uv = Math::Vector2f(1.0f, 0.0f);

		Vertex east_Top;
		east_Top.position = Math::Vector4f(0.0f, aSize.y, 0.0f, 1);
		east_Top.uv = Math::Vector2f(0.6f, 1.0f);	east_Top.normal = { 1.0f, 0.0f, 0.0f };

		normal = CalcualtePyramidNormal(east_West.position.AsVector3(), east_Top.position.AsVector3(), east_East.position.AsVector3());
		east_West.normal = normal;
		east_East.normal = normal;
		east_Top.normal = normal;

		Vertex north_West;
		north_West.position = Math::Vector4f(-aSize.x, -aSize.y, aSize.z, 1);
		north_West.uv = Math::Vector2f(0.0f, 0.0f);

		Vertex north_East;
		north_East.position = Math::Vector4f(aSize.x, -aSize.y, aSize.z, 1);
		north_East.uv = Math::Vector2f(1.0f, 0.0f);

		Vertex north_Top;
		north_Top.position = Math::Vector4f(0.0f, aSize.y, 0.0f, 1);
		north_Top.uv = Math::Vector2f(0.6f, 1.0f);

		normal = CalcualtePyramidNormal(north_East.position.AsVector3(), north_Top.position.AsVector3(), north_West.position.AsVector3());
		north_West.normal = normal;
		north_East.normal = normal;
		north_Top.normal = normal;

		Vertex west_West;
		west_West.position = Math::Vector4f(-aSize.x, -aSize.y, aSize.z, 1);
		west_West.uv = Math::Vector2f(0.0f, 0.0f);

		Vertex west_East;
		west_East.position = Math::Vector4f(-aSize.x, -aSize.y, -aSize.z, 1);
		west_East.uv = Math::Vector2f(1.0f, 0.0f);

		Vertex west_Top;
		west_Top.position = Math::Vector4f(0.0f, aSize.y, 0.0f, 1);
		west_Top.uv = Math::Vector2f(0.6f, 1.0f);

		normal = CalcualtePyramidNormal(west_West.position.AsVector3(), west_Top.position.AsVector3(), west_East.position.AsVector3());
		west_West.normal = normal;
		west_East.normal = normal;
		west_Top.normal = normal;

		Vertex southWest;
		southWest.position = south_West.position;
		southWest.uv = Math::Vector2f(0.0f, 0.0f);
		southWest.normal = { 0.0f, -1.0f, 0.0f };

		Vertex northWest;
		northWest.position = west_West.position;
		northWest.uv = Math::Vector2f(0.0f, 1.0f);
		northWest.normal = { 0.0f, -1.0f, 0.0f };

		Vertex northEast;
		northEast.position = east_East.position;
		northEast.uv = Math::Vector2f(1.0f, 1.0f);
		northEast.normal = { 0.0f, -1.0f, 0.0f };

		Vertex southEast;
		southEast.position = south_East.position;
		southEast.uv = Math::Vector2f(1.0f, 0.0f);
		southEast.normal = { 0.0f, -1.0f, 0.0f };

		std::vector<Vertex> vertices =
		{
			south_West, south_Top, south_East,
			east_West, east_Top, east_East,
			north_East, north_Top, north_West,
			west_West, west_Top, west_East,
			southWest, northWest, northEast, southEast
		};

		std::vector<unsigned int> indices =
		{
			//Base
			14,13,12,
			15,14,12,

			//Sides
			0,9, 1,
			0,1,2,
			3,4,5,
			6,7,8,
			9,10,11
		};

		return MeshData(vertices, indices);
	}

	MeshData ShapeCreator3000::CreateCube(const Math::Vector3f& aSize)
	{
		Vertex north_bottomLeft;
		north_bottomLeft.position = Math::Vector4f(aSize.x, -aSize.y, aSize.z, 1.0f);
		north_bottomLeft.uv = Math::Vector2f(0, 0);
		north_bottomLeft.normal = { 0.0f, 0.0f, 1.0f };

		Vertex north_bottomRight;
		north_bottomRight.position = Math::Vector4f(-aSize.x, -aSize.y, aSize.z, 1.0f);
		north_bottomRight.uv = Math::Vector2f(1, 0);
		north_bottomRight.normal = { 0.0f, 0.0f, 1.0f };

		Vertex north_topLeft;
		north_topLeft.position = Math::Vector4f(aSize.x, aSize.y, aSize.z, 1.0f);
		north_topLeft.uv = Math::Vector2f(0, 1);
		north_topLeft.normal = { 0.0f, 0.0f, 1.0f };

		Vertex north_topRight;
		north_topRight.position = Math::Vector4f(-aSize.x, aSize.y, aSize.z, 1.0f);
		north_topRight.uv = Math::Vector2f(1, 1);
		north_topRight.normal = { 0.0f, 0.0f, 1.0f };

		Vertex west_bottomLeft;
		west_bottomLeft.position = Math::Vector4f(-aSize.x, -aSize.y, aSize.z, 1.0f);
		west_bottomLeft.uv = Math::Vector2f(0, 0);
		west_bottomLeft.normal = { -1.0f, 0.0f, 0.0f };

		Vertex west_bottomRight;
		west_bottomRight.position = Math::Vector4f(-aSize.x, -aSize.y, -aSize.z, 1.0f);
		west_bottomRight.uv = Math::Vector2f(1, 0);
		west_bottomRight.normal = { -1.0f, 0.0f, 0.0f };

		Vertex west_topLeft;
		west_topLeft.position = Math::Vector4f(-aSize.x, aSize.y, aSize.z, 1.0f);
		west_topLeft.uv = Math::Vector2f(0, 1);
		west_topLeft.normal = { -1.0f, 0.0f, 0.0f };

		Vertex west_topRight;
		west_topRight.position = Math::Vector4f(-aSize.x, aSize.y, -aSize.z, 1.0f);
		west_topRight.uv = Math::Vector2f(1, 1);
		west_topRight.normal = { -1.0f, 0.0f, 0.0f };

		Vertex south_bottomLeft;
		south_bottomLeft.position = Math::Vector4f(-aSize.x, -aSize.y, -aSize.z, 1.0f);
		south_bottomLeft.uv = Math::Vector2f(0, 0);
		south_bottomLeft.normal = { 0.0f, 0.0f, -1.0f };

		Vertex south_bottomRight;
		south_bottomRight.position = Math::Vector4f(aSize.x, -aSize.y, -aSize.z, 1.0f);
		south_bottomRight.uv = Math::Vector2f(1, 0);
		south_bottomRight.normal = { 0.0f, 0.0f, -1.0f };

		Vertex south_topLeft;
		south_topLeft.position = Math::Vector4f(-aSize.x, aSize.y, -aSize.z, 1.0f);
		south_topLeft.uv = Math::Vector2f(0, 1);
		south_topLeft.normal = { 0.0f, 0.0f, -1.0f };

		Vertex south_topRight;
		south_topRight.position = Math::Vector4f(aSize.x, aSize.y, -aSize.z, 1.0f);
		south_topRight.uv = Math::Vector2f(1, 1);
		south_topRight.normal = { 0.0f, 0.0f, -1.0f };

		Vertex east_bottomLeft;
		east_bottomLeft.position = Math::Vector4f(aSize.x, -aSize.y, -aSize.z, 1.0f);
		east_bottomLeft.uv = Math::Vector2f(0, 0);
		east_bottomLeft.normal = { 1.0f, 0.0f, 0.0f };

		Vertex east_bottomRight;
		east_bottomRight.position = Math::Vector4f(aSize.x, -aSize.y, aSize.z, 1.0f);
		east_bottomRight.uv = Math::Vector2f(1, 0);
		east_bottomRight.normal = { 1.0f, 0.0f, 0.0f };

		Vertex east_topLeft;
		east_topLeft.position = Math::Vector4f(aSize.x, aSize.y, -aSize.z, 1.0f);
		east_topLeft.uv = Math::Vector2f(0, 1);
		east_topLeft.normal = { 1.0f, 0.0f, 0.0f };

		Vertex east_topRight;
		east_topRight.position = Math::Vector4f(aSize.x, aSize.y, aSize.z, 1.0f);
		east_topRight.uv = Math::Vector2f(1, 1);
		east_topRight.normal = { 1.0f, 0.0f, 0.0f };

		Vertex up_bottomLeft;
		up_bottomLeft.position = Math::Vector4f(aSize.x, aSize.y, aSize.z, 1.0f);
		up_bottomLeft.uv = Math::Vector2f(0, 0);
		up_bottomLeft.normal = { 0.0f, 1.0f, 0.0f };

		Vertex up_bottomRight;
		up_bottomRight.position = Math::Vector4f(-aSize.x, aSize.y, aSize.z, 1.0f);
		up_bottomRight.uv = Math::Vector2f(1, 0);
		up_bottomRight.normal = { 0.0f, 1.0f, 0.0f };

		Vertex up_topLeft;
		up_topLeft.position = Math::Vector4f(aSize.x, aSize.y, -aSize.z, 1.0f);
		up_topLeft.uv = Math::Vector2f(0, 1);
		up_topLeft.normal = { 0.0f, 1.0f, 0.0f };

		Vertex up_topRight;
		up_topRight.position = Math::Vector4f(-aSize.x, aSize.y, -aSize.z, 1.0f);
		up_topRight.uv = Math::Vector2f(1, 1);
		up_topRight.normal = { 0.0f, 1.0f, 0.0f };

		Vertex down_bottomLeft;
		down_bottomLeft.position = Math::Vector4f(-aSize.x, -aSize.y, aSize.z, 1.0f);
		down_bottomLeft.uv = Math::Vector2f(0, 0);
		down_bottomLeft.normal = { 0.0f, -1.0f, 0.0f };

		Vertex down_bottomRight;
		down_bottomRight.position = Math::Vector4f(aSize.x, -aSize.y, aSize.z, 1.0f);
		down_bottomRight.uv = Math::Vector2f(1, 0);
		down_bottomRight.normal = { 0.0f, -1.0f, 0.0f };

		Vertex down_topLeft;
		down_topLeft.position = Math::Vector4f(-aSize.x, -aSize.y, -aSize.z, 1.0f);
		down_topLeft.uv = Math::Vector2f(0, 1);
		down_topLeft.normal = { 0.0f, -1.0f, 0.0f };

		Vertex down_topRight;
		down_topRight.position = Math::Vector4f(aSize.x, -aSize.y, -aSize.z, 1.0f);
		down_topRight.uv = Math::Vector2f(1, 1);
		down_topRight.normal = { 0.0f, -1.0f, 0.0f };

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

	MeshData ShapeCreator3000::CreatePlane()
	{
		Vertex southWest;
		southWest.position = Math::Vector4f(-1.0f, 0.0f, -1.0f, 1.0f);
		southWest.color = Math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
		southWest.uv = Math::Vector2f(0.0f, 0.0f);
		southWest.normal = { 0.0f,1.0f,0.0f };

		Vertex southEast;
		southEast.position = Math::Vector4f(1.0f, 0.0f, -1.0f, 1.0f);
		southEast.color = Math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
		southEast.uv = Math::Vector2f(1.0f, 0.0f);
		southEast.normal = { 0.0f,1.0f,0.0f };

		Vertex northWest;
		northWest.position = Math::Vector4f(-1.0f, 0.0f, 1.0f, 1.0f);
		northWest.color = Math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
		northWest.uv = Math::Vector2f(0.0f, 1.0f);
		northWest.normal = { 0.0f,1.0f,0.0f };

		Vertex northEast;
		northEast.position = Math::Vector4f(1.0f, 0.0f, 1.0f, 1.0f);
		northEast.color = Math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
		northEast.uv = Math::Vector2f(1.0f, 1.0f);
		northEast.normal = { 0.0f,1.0f,0.0f };

		std::vector<Vertex> vertices =
		{
			southWest, southEast, northWest, northEast
		};

		std::vector<unsigned int> indices =
		{
			0, 2, 3,
			0, 3, 1
		};

		return MeshData(vertices, indices);
	}

	MeshData ShapeCreator3000::Create2DPlane()
	{
		Vertex southWest;
		southWest.position = Math::Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
		southWest.color = Math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
		southWest.uv = Math::Vector2f(0.0f, 0.0f);
		southWest.normal = { 0.0f,0.0f,-1.0f };

		Vertex southEast;
		southEast.position = Math::Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
		southEast.color = Math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
		southEast.uv = Math::Vector2f(1.0f, 0.0f);
		southEast.normal = { 0.0f,0.0f,-1.0f };

		Vertex northWest;
		northWest.position = Math::Vector4f(0.0f, 1.0f, 0.0f, 1.0f);
		northWest.color = Math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
		northWest.uv = Math::Vector2f(0.0f, 1.0f);
		northWest.normal = { 0.0f,0.0f,-1.0f };

		Vertex northEast;
		northEast.position = Math::Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
		northEast.color = Math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
		northEast.uv = Math::Vector2f(1.0f, 1.0f);
		northEast.normal = { 0.0f,0.0f,-1.0f };

		std::vector<Vertex> vertices =
		{
			southWest, southEast, northWest, northEast
		};

		std::vector<unsigned int> indices =
		{
			0, 2, 3,
			0, 3, 1
		};

		return MeshData(vertices, indices);
	}

	MeshData ShapeCreator3000::CreateSphere(const float aRadius, const int aSlices, const int aStacks)
	{
		//Dear Savior ChatGPT, I have absolutely no clue how this calculation work at all so don't ask
		std::vector<Vertex> vertices;

		for (int i = 0; i <= aStacks; ++i)
		{
			float phi = static_cast<float>(i) / aStacks * Math::globalPi;
			for (int j = 0; j <= aSlices; ++j)
			{
				float theta = static_cast<float>(j) * 2.0f * Math::globalPi / aSlices;

				Vertex vertex;
				vertex.position.x = aRadius * std::sin(phi) * std::cos(theta);
				vertex.position.y = aRadius * std::cos(phi);
				vertex.position.z = aRadius * std::sin(phi) * std::sin(theta);
				vertex.position.w = 1.0f;

				vertex.color = Math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
				vertex.uv = Math::Vector2f((4.0f + theta) / (2.0f * Math::globalPi), 1.0f - phi / Math::globalPi);

				vertices.push_back(vertex);
			}
		}

		std::vector<unsigned int> indices;

		for (int i = 0; i < aStacks; ++i)
		{
			for (int j = 0; j < aSlices; ++j)
			{
				int v1 = i * (aSlices + 1) + j;
				int v2 = v1 + 1;
				int v3 = (i + 1) * (aSlices + 1) + j;
				int v4 = v3 + 1;

				indices.push_back(v2);
				indices.push_back(v3);
				indices.push_back(v1);

				indices.push_back(v4);
				indices.push_back(v3);
				indices.push_back(v2);
			}
		}

		return MeshData(vertices, indices);
	}

	Math::Vector3f ShapeCreator3000::CalcualtePyramidNormal(const Math::Vector3f& aVertexA, const Math::Vector3f& aVertexB, const Math::Vector3f& aVertexC)
	{
		const Math::Vector3f BA = aVertexB - aVertexA;
		const Math::Vector3f CB = aVertexC - aVertexB;
		const Math::Vector3f normal = Math::Cross(BA, CB).GetNormalized();

		return normal;
	}

	MeshData ShapeCreator3000::CreateSkyBox(const Math::Vector3f& aSize)
	{
		Vertex north_bottomLeft;
		north_bottomLeft.position = Math::Vector4f(aSize.x, -aSize.y, aSize.z, 1.0f);
		north_bottomLeft.uv = Math::Vector2f(0, 0);

		Vertex north_bottomRight;
		north_bottomRight.position = Math::Vector4f(-aSize.x, -aSize.y, aSize.z, 1.0f);
		north_bottomRight.uv = Math::Vector2f(1, 0);

		Vertex north_topLeft;
		north_topLeft.position = Math::Vector4f(aSize.x, aSize.y, aSize.z, 1.0f);
		north_topLeft.uv = Math::Vector2f(0, 1);

		Vertex north_topRight;
		north_topRight.position = Math::Vector4f(-aSize.x, aSize.y, aSize.z, 1.0f);
		north_topRight.uv = Math::Vector2f(1, 1);

		Vertex west_bottomLeft;
		west_bottomLeft.position = Math::Vector4f(-aSize.x, -aSize.y, aSize.z, 1.0f);
		west_bottomLeft.uv = Math::Vector2f(0, 0);

		Vertex west_bottomRight;
		west_bottomRight.position = Math::Vector4f(-aSize.x, -aSize.y, -aSize.z, 1.0f);
		west_bottomRight.uv = Math::Vector2f(1, 0);

		Vertex west_topLeft;
		west_topLeft.position = Math::Vector4f(-aSize.x, aSize.y, aSize.z, 1.0f);
		west_topLeft.uv = Math::Vector2f(0, 1);

		Vertex west_topRight;
		west_topRight.position = Math::Vector4f(-aSize.x, aSize.y, -aSize.z, 1.0f);
		west_topRight.uv = Math::Vector2f(1, 1);

		Vertex south_bottomLeft;
		south_bottomLeft.position = Math::Vector4f(-aSize.x, -aSize.y, -aSize.z, 1.0f);
		south_bottomLeft.uv = Math::Vector2f(0, 0);

		Vertex south_bottomRight;
		south_bottomRight.position = Math::Vector4f(aSize.x, -aSize.y, -aSize.z, 1.0f);
		south_bottomRight.uv = Math::Vector2f(1, 0);

		Vertex south_topLeft;
		south_topLeft.position = Math::Vector4f(-aSize.x, aSize.y, -aSize.z, 1.0f);
		south_topLeft.uv = Math::Vector2f(0, 1);

		Vertex south_topRight;
		south_topRight.position = Math::Vector4f(aSize.x, aSize.y, -aSize.z, 1.0f);
		south_topRight.uv = Math::Vector2f(1, 1);

		Vertex east_bottomLeft;
		east_bottomLeft.position = Math::Vector4f(aSize.x, -aSize.y, -aSize.z, 1.0f);
		east_bottomLeft.uv = Math::Vector2f(0, 0);

		Vertex east_bottomRight;
		east_bottomRight.position = Math::Vector4f(aSize.x, -aSize.y, aSize.z, 1.0f);
		east_bottomRight.uv = Math::Vector2f(1, 0);

		Vertex east_topLeft;
		east_topLeft.position = Math::Vector4f(aSize.x, aSize.y, -aSize.z, 1.0f);
		east_topLeft.uv = Math::Vector2f(0, 1);

		Vertex east_topRight;
		east_topRight.position = Math::Vector4f(aSize.x, aSize.y, aSize.z, 1.0f);
		east_topRight.uv = Math::Vector2f(1, 1);

		Vertex up_bottomLeft;
		up_bottomLeft.position = Math::Vector4f(aSize.x, aSize.y, aSize.z, 1.0f);
		up_bottomLeft.uv = Math::Vector2f(0, 0);

		Vertex up_bottomRight;
		up_bottomRight.position = Math::Vector4f(-aSize.x, aSize.y, aSize.z, 1.0f);
		up_bottomRight.uv = Math::Vector2f(1, 0);

		Vertex up_topLeft;
		up_topLeft.position = Math::Vector4f(aSize.x, aSize.y, -aSize.z, 1.0f);
		up_topLeft.uv = Math::Vector2f(0, 1);

		Vertex up_topRight;
		up_topRight.position = Math::Vector4f(-aSize.x, aSize.y, -aSize.z, 1.0f);
		up_topRight.uv = Math::Vector2f(1, 1);

		Vertex down_bottomLeft;
		down_bottomLeft.position = Math::Vector4f(-aSize.x, -aSize.y, aSize.z, 1.0f);
		down_bottomLeft.uv = Math::Vector2f(0, 0);

		Vertex down_bottomRight;
		down_bottomRight.position = Math::Vector4f(aSize.x, -aSize.y, aSize.z, 1.0f);
		down_bottomRight.uv = Math::Vector2f(1, 0);

		Vertex down_topLeft;
		down_topLeft.position = Math::Vector4f(-aSize.x, -aSize.y, -aSize.z, 1.0f);
		down_topLeft.uv = Math::Vector2f(0, 1);

		Vertex down_topRight;
		down_topRight.position = Math::Vector4f(aSize.x, -aSize.y, -aSize.z, 1.0f);
		down_topRight.uv = Math::Vector2f(1, 1);

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
}