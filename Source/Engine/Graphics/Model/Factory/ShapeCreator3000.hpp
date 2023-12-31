#pragma once

namespace Simple
{
	struct TerrainData
	{
		float amplitude = 5.0f;
		float vertexLength = 0.50f;
		unsigned int gridSize = 12;
		unsigned int upSampleMultiplier = 2;
	};
}

namespace Simple
{
	class ShapeCreator3000 final
	{
	public:
		static MeshData CreateTerrain(const Simple::TerrainData& aTerrainData = Simple::TerrainData{});
		static MeshData CreatePyramid(const SimpleUtilities::Vector3f& aSize = { 1.0f , 1.0f, 1.0f });
		static MeshData CreateCube(const SimpleUtilities::Vector3f& aSize = { 1.0f, 1.0f , 1.0f });
		static MeshData CreateSkyBox(const SimpleUtilities::Vector3f& aSize = { 1000.0f, 1000.0f, 1000.0f });
		static MeshData CreatePlane();
		static MeshData CreateSphere(const float aRadius = 1.0f, const int aSlices = 20, const int aStacks = 20);
	private:
		static SimpleUtilities::Vector3f CalcualtePyramidNormal(const SimpleUtilities::Vector3f& aVertexA, const SimpleUtilities::Vector3f& aVertexB, const SimpleUtilities::Vector3f& aVertexC);
	};
}